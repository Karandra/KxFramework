#include "stdafx.h"
#include "Archive.h"
#include "Library.h"
#include "Common.h"
#include "Private/Utility.h"
#include "Private/GUIDs.h"
#include "Private/ArchiveOpenCallback.h"
#include "Private/ArchiveExtractCallback.h"
#include "Private/ArchiveUpdateCallback.h"
#include "Private/InStreamWrapper.h"
#include "Private/OutStreamWrapper.h"
#include "kxf/System/ErrorCode.h"
#include "kxf/System/VariantProperty.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include "kxf/Utility/TypeTraits.h"

namespace
{
	using namespace kxf;
	using namespace kxf::SevenZip;
	using ArchiveProperty = decltype(kpidNoProperty);

	NativeFileSystem g_NativeFS;

	const char* GetMethodString(CompressionMethod method) noexcept
	{
		switch (method)
		{
			case CompressionMethod::LZMA:
			{
				return "LZMA";
			}
			case CompressionMethod::BZIP2:
			{
				return "BZip2";
			}
			case CompressionMethod::PPMD:
			{
				return "PPMd";
			}
		};

		// LZMA2 is the default
		return "LZMA2";
	}
	std::string FormatMethodString(int dictionarySize, SevenZip::CompressionMethod method)
	{
		const int64_t dictSizeMB = 20 + dictionarySize;

		std::string result(256, '\000');
		switch (method)
		{
			case CompressionMethod::LZMA:
			case CompressionMethod::LZMA2:
			{
				sprintf_s(result.data(), result.size(), "%s:d=%lld", GetMethodString(method), dictSizeMB);
				break;
			}
			case CompressionMethod::BZIP2:
			{
				result = "BZip2:d=900000b";
				break;
			}
			case CompressionMethod::PPMD:
			{
				sprintf_s(result.data(), result.size(), "%s:mem=%lld", GetMethodString(method), dictSizeMB);
				break;
			}
		};
		return result;
	}
	bool SetCompressionProperties(IUnknown& archive, bool multithreaded, bool solidArchive, int compressionLevel, int dictionarySize, SevenZip::CompressionMethod method)
	{
		std::string methodString = FormatMethodString(dictionarySize, method);
		constexpr const wchar_t* names[] = {L"x", L"s", L"mt", L"m"};
		VariantProperty values[] =
		{
			static_cast<uint32_t>(compressionLevel),
			solidArchive,
			multithreaded,
			methodString.c_str()
		};

		COMPtr<ISetProperties> propertiesSet;
		archive.QueryInterface(COM::ToGUID(SevenZip::GUID::IID_ISetProperties), reinterpret_cast<void**>(&propertiesSet));
		if (!propertiesSet)
		{
			// Archive does not support setting compression properties
			return false;
		}

		return HResult(propertiesSet->SetProperties(names, reinterpret_cast<const PROPVARIANT*>(values), std::size(values))).IsSuccess();
	}

	template<class T>
	std::optional<T> GetIntProperty(const IInArchive& archive, size_t fileIndex, ArchiveProperty type)
	{
		VariantProperty property;
		if (HResult(const_cast<IInArchive&>(archive).GetProperty(static_cast<uint32_t>(fileIndex), type, &property)))
		{
			return property.ToInt<T>();
		}
		return {};
	}
}

namespace kxf::SevenZip
{
	void Archive::InvalidateCache()
	{
		m_IsLoaded = false;
	}
	bool Archive::InitCompressionFormat()
	{
		m_OverrideCompressionFormat = false;
		m_Property_CompressionFormat = Private::IdentifyCompressionFormat(m_ArchivePath, m_EvtHandler);

		return m_Property_CompressionFormat != CompressionFormat::Unknown;
	}
	bool Archive::InitMetadata()
	{
		if (!m_IsLoaded)
		{
			return !m_OverrideCompressionFormat ? InitCompressionFormat() : true;
		}
		return m_IsLoaded;
	}
	bool Archive::InitArchiveStreams()
	{
		m_ArchiveStream = nullptr;
		m_ArchiveStreamWrapper = nullptr;
		m_ArchiveStreamReader = nullptr;
		m_ItemCount = 0;

		m_ArchiveStream = Private::OpenFileToRead(m_ArchivePath);
		if (m_ArchiveStream)
		{
			m_ArchiveStreamWrapper = Private::CreateObject<Private::InStreamWrapper>(m_ArchiveStream, m_EvtHandler);
			m_ArchiveStreamReader = Private::GetArchiveReader(m_Property_CompressionFormat);

			if (m_ArchiveStreamReader)
			{
				auto openCallback = Private::CreateObject<Private::Callback::OpenArchive>(m_EvtHandler);
				if (SUCCEEDED(m_ArchiveStreamReader->Open(m_ArchiveStreamWrapper, nullptr, openCallback)))
				{
					m_ItemCount = Private::GetNumberOfItems(m_ArchiveStreamReader).value_or(0);
					return true;
				}
			}
		}
		return false;
	}
	void Archive::RewindArchiveStreams()
	{
		auto SeekStream = [](IStream& stream, int64_t position)
		{
			LARGE_INTEGER value = {};
			value.QuadPart = position;

			return stream.Seek(value, STREAM_SEEK_SET, nullptr);
		};
		auto SeekStreamWrapper = [](IInStream& stream, int64_t position)
		{
			return stream.Seek(position, STREAM_SEEK_SET, nullptr);
		};

		SeekStream(*m_ArchiveStream, 0);
		SeekStreamWrapper(*m_ArchiveStreamWrapper, 0);
	}

	bool Archive::DoExtract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView* files) const
	{
		if (files && files->empty())
		{
			return false;
		}

		if (auto fileStream = Private::OpenFileToRead(m_ArchivePath))
		{
			auto archive = Private::GetArchiveReader(m_Property_CompressionFormat);
			auto inFile = Private::CreateObject<Private::InStreamWrapper>(fileStream, m_EvtHandler);
			auto openCallback = Private::CreateObject<Private::Callback::OpenArchive>(m_EvtHandler);

			if (HResult(archive->Open(inFile, nullptr, openCallback)))
			{
				Utility::CallAtScopeExit atExit = ([&]()
				{
					archive->Close();
				});
				extractor->SetArchive(archive);
				extractor->SetEvtHandler(m_EvtHandler);

				HResult result = HResult::Fail();
				if (files)
				{
					auto IsInvalidIndex = [this](size_t index)
					{
						return index >= m_ItemCount;
					};

					// Process only specified files
					if (files->size() == 1)
					{
						// No need to sort single index
						if (IsInvalidIndex(files->front()))
						{
							result = HResult::InvalidArgument();
							return false;
						}

						uint32_t index = files->front();
						result = archive->Extract(&index, 1, false, extractor);
					}
					else
					{
						auto temp = files->ToVector<uint32_t>();

						// Remove invalid items
						temp.erase(std::remove_if(temp.begin(), temp.end(), IsInvalidIndex), temp.end());

						// IInArchive::Extract requires sorted array
						std::sort(temp.begin(), temp.end());

						result = HResult::InvalidArgument();
						if (!temp.empty())
						{
							result = archive->Extract(temp.data(), static_cast<uint32_t>(temp.size()), false, extractor);
						}
					}
				}
				else
				{
					// Process all files, the callback will decide which files are needed
					result = archive->Extract(nullptr, std::numeric_limits<uint32_t>::max(), false, extractor);
				}

				return result.IsSuccess();
			}
		}
		return false;
	}
	bool Archive::DoCompress(const FSPath& pathPrefix, const std::vector<FileItem>& filePaths, const std::vector<FSPath>& inArchiveFilePaths)
	{
		auto archiveWriter = Private::GetArchiveWriter(m_Property_CompressionFormat);
		if (archiveWriter)
		{
			SetCompressionProperties(*archiveWriter,
									 m_Property_MultiThreaded,
									 m_Property_Solid,
									 static_cast<int>(m_Property_CompressionLevel),
									 m_Property_DictionarySize,
									 m_Property_CompressionMethod);

			auto outFile = Private::CreateObject<Private::OutStreamWrapper_IStream>(Private::OpenFileToWrite(m_ArchivePath));
			if (outFile)
			{
				auto updateCallback = Private::CreateObject<Private::Callback::UpdateArchive>(pathPrefix, filePaths, inArchiveFilePaths, m_ArchivePath, m_EvtHandler);
				updateCallback->SetExistingItemsCount(m_ItemCount);

				return HResult(archiveWriter->UpdateItems(outFile, static_cast<uint32_t>(filePaths.size()), updateCallback)).IsSuccess();
			}
		}
		return false;
	}
	bool Archive::FindAndCompressFiles(const IFileSystem& fileSystem, const FSPath& directory, const FSPathQuery& searchPattern, const FSPath& pathPrefix, bool recursion)
	{
		std::vector<FileItem> files;
		std::vector<FSPath> relativePaths;

		if (fileSystem.EnumItems(directory, [&](FileItem item)
		{
			relativePaths.emplace_back(item.GetFullPath().GetAfter(directory));
			files.emplace_back(std::move(item));

			return true;
		}, searchPattern, FlagSet<FSEnumItemsFlag>(FSEnumItemsFlag::LimitToFiles).Add(FSEnumItemsFlag::Recursive, recursion)) != 0)
		{
			return DoCompress(pathPrefix, files, relativePaths);
		}
		return false;
	}

	Archive::Archive(wxEvtHandler* evtHandler)
		:WithEvtHandler(evtHandler)
	{
	}
	Archive::~Archive() = default;

	bool Archive::Load(FSPath filePath)
	{
		// Clear metadata
		*this = std::move(Archive(m_EvtHandler));

		// Load new archive
		m_ArchivePath = std::move(filePath);
		m_IsLoaded = InitMetadata() && InitArchiveStreams();

		return m_IsLoaded;
	}
	FileItem Archive::GetItem(size_t index) const
	{
		if (index < m_ItemCount)
		{
			return Private::GetArchiveItem(m_ArchiveStreamReader, index);
		}
		return {};
	}

	int64_t Archive::GetOriginalSize() const
	{
		int64_t total = 0;
		for (size_t i = 0; i < m_ItemCount; i++)
		{
			total += GetIntProperty<int64_t>(*m_ArchiveStreamReader, i, ArchiveProperty::kpidSize).value_or(0);
		}
		return total;
	}
	int64_t Archive::GetCompressedSize() const
	{
		int64_t total = 0;
		for (size_t i = 0; i < m_ItemCount; i++)
		{
			total += GetIntProperty<int64_t>(*m_ArchiveStreamReader, i, ArchiveProperty::kpidPackSize).value_or(0);
		}
		return total;
	}

	// Extraction
	bool Archive::Extract(COMPtr<Private::Callback::ExtractArchive> extractor) const
	{
		return DoExtract(std::move(extractor), nullptr);
	}
	bool Archive::Extract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView files) const
	{
		return DoExtract(std::move(extractor), &files);
	}

	bool Archive::ExtractToDirectory(const FSPath& directory) const
	{
		auto extractor = Private::CreateObject<Private::Callback::ExtractArchiveToDisk>(directory, m_EvtHandler);
		return DoExtract(std::move(extractor), nullptr);
	}
	bool Archive::ExtractToDirectory(const FSPath& directory, Compression::FileIndexView files) const
	{
		auto extractor = Private::CreateObject<Private::Callback::ExtractArchiveToDisk>(directory, m_EvtHandler);
		return DoExtract(std::move(extractor), &files);
	}

	// Compression
	bool Archive::CompressDirectory(const FSPath& directory, bool isRecursive)
	{
		return FindAndCompressFiles(g_NativeFS, directory, {}, directory, isRecursive);
	}
	bool Archive::CompressFiles(const FSPath& directory, const FSPath& searchFilter, bool recursive)
	{
		return FindAndCompressFiles(g_NativeFS, directory, searchFilter, directory, recursive);
	}
	bool Archive::CompressSpecifiedFiles(const std::vector<FSPath>& sourceFiles, const std::vector<FSPath>& archivePaths)
	{
		std::vector<FileItem> files;
		files.reserve(sourceFiles.size());

		for (const FSPath& path: sourceFiles)
		{
			files.emplace_back(g_NativeFS.GetItem(path));
		}

		return DoCompress({}, files, archivePaths);
	}
	bool Archive::CompressFile(const FSPath& filePath)
	{
		return DoCompress(filePath.GetParent(), {filePath}, {});
	}
	bool Archive::CompressFile(const FSPath& filePath, const FSPath& archivePath)
	{
		return DoCompress(filePath.GetParent(), {filePath}, {archivePath});
	}

	Archive& Archive::operator=(Archive&& other)
	{
		SetEvtHandler(other.GetEvtHandler());
		other.SetEvtHandler(nullptr);

		m_ArchivePath = std::move(other.m_ArchivePath);
		m_ArchiveStream = std::move(other.m_ArchiveStream);
		m_ArchiveStreamReader = std::move(other.m_ArchiveStreamReader);
		m_ArchiveStreamWrapper = std::move(other.m_ArchiveStreamWrapper);

		// Metadata
		Utility::ExchangeAndReset(m_ItemCount, other.m_ItemCount, 0);
		Utility::ExchangeAndReset(m_IsLoaded, other.m_IsLoaded, false);
		Utility::ExchangeAndReset(m_OverrideCompressionFormat, other.m_OverrideCompressionFormat, false);

		return *this;
	}
}
