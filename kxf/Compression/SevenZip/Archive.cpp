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
	std::optional<T> DoGetIntProperty(const IInArchive& archive, size_t fileIndex, ArchiveProperty type)
	{
		VariantProperty property;
		if (HResult(const_cast<IInArchive&>(archive).GetProperty(static_cast<uint32_t>(fileIndex), type, &property)))
		{
			return property.ToInt<T>();
		}
		return {};
	}

	FlagSet<uint32_t> GetItemAttributes(const IInArchive& stream, size_t itemCount, const UniversallyUniqueID& id) noexcept
	{
		auto luid = id.ToLocallyUniqueID();
		if (luid < itemCount)
		{
			VariantProperty property;
			if (HResult(const_cast<IInArchive&>(stream).GetProperty(static_cast<size_t>(luid.ToInt()), kpidAttrib, &property)))
			{
				return property.ToInt<uint32_t>().value_or(INVALID_FILE_ATTRIBUTES);
			}
		}
		return INVALID_FILE_ATTRIBUTES;
	}

	size_t FindItemByName(const IInArchive& stream, size_t itemCount, const FSPath& path)
	{
		for (size_t i = 0; i < itemCount; i++)
		{
			VariantProperty property;

			const_cast<IInArchive&>(stream).GetProperty(i, kpidPath, &property);
			if (auto itemPath = property.ToString())
			{
				if (path.IsSameAs(*itemPath))
				{
					return i;
				}
			}
		}
		return std::numeric_limits<size_t>::max();
	}
}

namespace kxf::SevenZip
{
	void Archive::InvalidateCache()
	{
		m_Data.IsLoaded = false;
	}
	bool Archive::InitCompressionFormat()
	{
		m_Data.OverrideCompressionFormat = false;
		m_Data.Properties.CompressionFormat = Private::IdentifyCompressionFormat(m_Data.FilePath, m_EvtHandler);

		return m_Data.Properties.CompressionFormat != CompressionFormat::Unknown;
	}
	bool Archive::InitMetadata()
	{
		if (!m_Data.IsLoaded)
		{
			return !m_Data.OverrideCompressionFormat ? InitCompressionFormat() : true;
		}
		return m_Data.IsLoaded;
	}
	bool Archive::InitArchiveStreams()
	{
		m_Data.ArchiveStream = nullptr;
		m_Data.ArchiveStreamWrapper = nullptr;
		m_Data.ArchiveStreamReader = nullptr;
		m_Data.ItemCount = 0;

		m_Data.ArchiveStream = Private::OpenFileToRead(m_Data.FilePath);
		if (m_Data.ArchiveStream)
		{
			m_Data.ArchiveStreamWrapper = Private::CreateObject<Private::InStreamWrapper>(m_Data.ArchiveStream, m_EvtHandler);
			m_Data.ArchiveStreamReader = Private::GetArchiveReader(m_Data.Properties.CompressionFormat);

			if (m_Data.ArchiveStreamReader)
			{
				auto openCallback = Private::CreateObject<Private::Callback::OpenArchive>(m_EvtHandler);
				if (HResult(m_Data.ArchiveStreamReader->Open(m_Data.ArchiveStreamWrapper, nullptr, openCallback)))
				{
					m_Data.ItemCount = Private::GetNumberOfItems(m_Data.ArchiveStreamReader).value_or(0);
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

		SeekStream(*m_Data.ArchiveStream, 0);
		SeekStreamWrapper(*m_Data.ArchiveStreamWrapper, 0);
	}

	bool Archive::DoExtract(COMPtr<Private::Callback::ExtractArchive> extractor, Compression::FileIndexView* files) const
	{
		if (files && files->empty())
		{
			return false;
		}

		if (auto fileStream = Private::OpenFileToRead(m_Data.FilePath))
		{
			auto archive = Private::GetArchiveReader(m_Data.Properties.CompressionFormat);
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
						return index >= m_Data.ItemCount;
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
		auto archiveWriter = Private::GetArchiveWriter(m_Data.Properties.CompressionFormat);
		if (archiveWriter)
		{
			SetCompressionProperties(*archiveWriter,
									 m_Data.Properties.MultiThreaded,
									 m_Data.Properties.Solid,
									 static_cast<int>(m_Data.Properties.CompressionLevel),
									 m_Data.Properties.DictionarySize,
									 m_Data.Properties.CompressionMethod);

			auto outFile = Private::CreateObject<Private::OutStreamWrapper_IStream>(Private::OpenFileToWrite(m_Data.FilePath));
			if (outFile)
			{
				auto updateCallback = Private::CreateObject<Private::Callback::UpdateArchive>(pathPrefix, filePaths, inArchiveFilePaths, m_Data.FilePath, m_EvtHandler);
				updateCallback->SetExistingItemsCount(m_Data.ItemCount);

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

	// IArchive
	bool Archive::Open(const FSPath& filePath)
	{
		Close();
		m_Data.FilePath = filePath;
		m_Data.IsLoaded = InitMetadata() && InitArchiveStreams();

		return m_Data.IsLoaded;
	}
	void Archive::Close()
	{
		m_Data = Data();
	}

	BinarySize Archive::GetOriginalSize() const
	{
		if (m_Data.OriginalSize < 0)
		{
			int64_t total = 0;
			for (size_t i = 0; i < m_Data.ItemCount; i++)
			{
				total += DoGetIntProperty<int64_t>(*m_Data.ArchiveStreamReader, i, ArchiveProperty::kpidSize).value_or(0);
			}
			m_Data.OriginalSize = total;
		}
		return m_Data.OriginalSize;
	}
	BinarySize Archive::GetCompressedSize() const
	{
		if (m_Data.CompressedSize < 0)
		{
			int64_t total = 0;
			for (size_t i = 0; i < m_Data.ItemCount; i++)
			{
				total += DoGetIntProperty<int64_t>(*m_Data.ArchiveStreamReader, i, ArchiveProperty::kpidPackSize).value_or(0);
			}
			m_Data.CompressedSize = total;
		}
		return m_Data.CompressedSize;
	}

	// IArchiveExtraction
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

	// IArchiveCompression
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

	// IFileSystem
	bool Archive::ItemExist(const FSPath& path) const
	{
		return FindItemByName(*m_Data.ArchiveStreamReader, m_Data.ItemCount, path) != std::numeric_limits<size_t>::max();
	}
	bool Archive::FileExist(const FSPath& path) const
	{
		size_t index = FindItemByName(*m_Data.ArchiveStreamReader, m_Data.ItemCount, path);
		if (index != std::numeric_limits<size_t>::max())
		{
			return !GetItemAttributes(*m_Data.ArchiveStreamReader, m_Data.ItemCount, LocallyUniqueID(index)).Contains(FILE_ATTRIBUTE_DIRECTORY);
		}
		return {};
	}
	bool Archive::DirectoryExist(const FSPath& path) const
	{
		size_t index = FindItemByName(*m_Data.ArchiveStreamReader, m_Data.ItemCount, path);
		if (index != std::numeric_limits<size_t>::max())
		{
			return GetItemAttributes(*m_Data.ArchiveStreamReader, m_Data.ItemCount, LocallyUniqueID(index)).Contains(FILE_ATTRIBUTE_DIRECTORY);
		}
		return {};
	}

	FileItem Archive::GetItem(const FSPath& path) const
	{
		size_t index = FindItemByName(*m_Data.ArchiveStreamReader, m_Data.ItemCount, path);
		if (index != std::numeric_limits<size_t>::max())
		{
			return Private::GetArchiveItem(m_Data.ArchiveStreamReader, index);
		}
		return {};
	}
	size_t Archive::EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query, FlagSet<FSEnumItemsFlag> flags) const
	{
		FlagSet<StringOpFlag> matchFlags;
		matchFlags.Add(StringOpFlag::IgnoreCase, !flags.Contains(FSEnumItemsFlag::CaseSensitive));

		auto SearchDirectory = [&](const FSPath& directory, std::vector<FSPath>& childDirectories)
		{
			size_t count = 0;
			EnumItems(std::numeric_limits<UniversallyUniqueID>::max(), [&](FileItem item)
			{
				FSPath fullPath = item.GetFullPath();
				FSPath relativePath = fullPath.GetAfter(directory);
				const bool hasChildItems = relativePath.GetComponentCount() > 1;

				bool result = true;
				if (relativePath.MatchesWildcards(query, matchFlags))
				{
					count++;
					result = std::invoke(func, std::move(item));
				}
				if (hasChildItems && flags & FSEnumItemsFlag::Recursive)
				{
					childDirectories.emplace_back(std::move(fullPath));
				}
				return result;
			}, flags);
			return count;
		};

		std::vector<FSPath> directories;
		size_t count = SearchDirectory(directory, directories);

		while (!directories.empty())
		{
			std::vector<FSPath> roundDirectories;
			for (const FSPath& path: directories)
			{
				count += SearchDirectory(path, roundDirectories);
			}
			directories = std::move(roundDirectories);
		}
		return count;
	}
	bool Archive::IsDirectoryEmpty(const FSPath& directory) const
	{
		return EnumItems(directory, [](const FileItem&)
		{
			return false;
		}, {}) == 0;
	}

	// IFileIDSystem
	bool Archive::ItemExist(const UniversallyUniqueID& id) const
	{
		auto attributes = GetItemAttributes(*m_Data.ArchiveStreamReader, m_Data.ItemCount, id);
		return !attributes.Equals(INVALID_FILE_ATTRIBUTES);
	}
	bool Archive::FileExist(const UniversallyUniqueID& id) const
	{
		auto attributes = GetItemAttributes(*m_Data.ArchiveStreamReader, m_Data.ItemCount, id);
		return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && !attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
	}
	bool Archive::DirectoryExist(const UniversallyUniqueID& id) const
	{
		auto attributes = GetItemAttributes(*m_Data.ArchiveStreamReader, m_Data.ItemCount, id);
		return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
	}

	FileItem Archive::GetItem(const UniversallyUniqueID& id) const
	{
		auto luid = id.ToLocallyUniqueID();
		if (luid < m_Data.ItemCount)
		{
			return Private::GetArchiveItem(m_Data.ArchiveStreamReader, static_cast<size_t>(luid.ToInt()));
		}
		return {};
	}
	size_t Archive::EnumItems(const UniversallyUniqueID& id, TEnumItemsFunc func, FlagSet<FSEnumItemsFlag> flags) const
	{
		if (id == std::numeric_limits<UniversallyUniqueID>::max())
		{
			size_t count = 0;
			for (size_t i = 0; i < m_Data.ItemCount; i++)
			{
				auto attributes = GetItemAttributes(*m_Data.ArchiveStreamReader, m_Data.ItemCount, LocallyUniqueID(i));
				if (flags & FSEnumItemsFlag::LimitToFiles && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY))
				{
					continue;
				}
				if (flags & FSEnumItemsFlag::LimitToDirectories && !attributes.Contains(FILE_ATTRIBUTE_DIRECTORY))
				{
					continue;
				}

				count++;
				if (!std::invoke(func, Private::GetArchiveItem(m_Data.ArchiveStreamReader, i)))
				{
					break;
				}
			}
			return count;
		}
		else if (auto luid = id.ToLocallyUniqueID(); luid < m_Data.ItemCount)
		{
			// TODO
		}
		return 0;
	}
	bool Archive::IsDirectoryEmpty(const UniversallyUniqueID& id) const
	{
		return EnumItems(id, [](const FileItem&)
		{
			return false;
		}, {}) == 0;
	}

	Archive& Archive::operator=(Archive&& other)
	{
		SetEvtHandler(other.GetEvtHandler());
		other.SetEvtHandler(nullptr);

		m_Data = std::move(other.m_Data);

		return *this;
	}
}