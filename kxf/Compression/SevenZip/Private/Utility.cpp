#include "stdafx.h"
#include "Utility.h"
#include "GUIDs.h"
#include "InStreamWrapper.h"
#include "ArchiveOpenCallback.h"
#include "../Library.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include <ShlObj.h>
#include <shlwapi.h>
#include "kxf/System/UndefWindows.h"

namespace
{
	using namespace kxf;

	COMPtr<IStream> CreateStreamOnFile(const FSPath& path, uint32_t flags)
	{
		COMPtr<IStream> fileStream;

		String pathName = path.GetFullPathWithNS(FSPathNamespace::Win32File);
		if (HResult(::SHCreateStreamOnFileEx(pathName.wc_str(), flags, FILE_ATTRIBUTE_NORMAL, flags & STGM_CREATE, nullptr, &fileStream)))
		{
			return fileStream;
		}
		return nullptr;
	}
}

namespace kxf::SevenZip::Private
{
	COMPtr<IInArchive> GetArchiveReader(CompressionFormat format) noexcept
	{
		if (NativeUUID guid = GetAlgorithmID(format))
		{
			return Library::GetInstance().CreateObject<IInArchive>(guid, GUID::IID_IInArchive);
		}
		return nullptr;
	}
	COMPtr<IOutArchive> GetArchiveWriter(CompressionFormat format) noexcept
	{
		if (NativeUUID guid = GetAlgorithmID(format))
		{
			return Library::GetInstance().CreateObject<IOutArchive>(guid, GUID::IID_IOutArchive);
		}
		return nullptr;
	}

	std::optional<size_t> GetNumberOfItems(const IInArchive& archive) noexcept
	{
		return [&](IInArchive& archive) -> std::optional<size_t>
		{
			uint32_t itemCount = 0;
			if (archive.GetNumberOfItems(&itemCount) == S_OK)
			{
				// If there are items try to verify if they're readable
				if (itemCount != 0)
				{
					VariantProperty property;
					if (HResult(archive.GetProperty(0, kpidSize, &property)) && !property.IsEmpty())
					{
						return itemCount;
					}
				}
			}
			return {};
		}(const_cast<IInArchive&>(archive));
	}
	bool GetNumberOfItems(wxInputStream& stream, CompressionFormat format, size_t& itemCount, EvtHandler* evtHandler)
	{
		if (stream.IsOk())
		{
			auto archive = GetArchiveReader(format);
			auto openCallback = COM::CreateLocalInstance<Callback::OpenArchive>(evtHandler);
			auto streamWrapper = COM::CreateLocalInstance<InStreamWrapper_wxInputStream>(stream, evtHandler);

			if (HResult(archive->Open(streamWrapper, nullptr, openCallback)))
			{
				Utility::CallAtScopeExit atExit([&]()
				{
					archive->Close();
				});
				if (auto count = GetNumberOfItems(*archive))
				{
					itemCount = *count;
					return true;
				}
			}
		}
		return false;
	}

	FileItem GetArchiveItem(const IInArchive& archive, size_t fileIndex)
	{
		return [&](IInArchive& archive) -> FileItem
		{
			FileItem fileItem;
			VariantProperty property;

			// Get name of file
			if (HResult(archive.GetProperty(fileIndex, kpidPath, &property)))
			{
				fileItem.SetFullPath(property.ToString().value_or(NullString));

				// Attributes
				if (FAILED(archive.GetProperty(fileIndex, kpidAttrib, &property)))
				{
					return {};
				}
				auto attributes = FileSystem::Private::MapFileAttributes(property.ToInt<uint32_t>().value_or(0));

				// Is directory
				if (SUCCEEDED(archive.GetProperty(fileIndex, kpidIsDir, &property)))
				{
					attributes.Add(FileAttribute::Directory, property.ToBool().value_or(false));
				}
				else
				{
					return {};
				}

				if (!attributes.Contains(FileAttribute::Directory))
				{
					// Original size
					if (SUCCEEDED(archive.GetProperty(fileIndex, kpidSize, &property)))
					{
						fileItem.SetSize(property.ToInt<BinarySize::SizeType>().value_or(-1));
					}
					else
					{
						return {};
					}

					// Compressed size
					if (SUCCEEDED(archive.GetProperty(fileIndex, kpidPackSize, &property)))
					{
						fileItem.SetCompressedSize(property.ToInt<BinarySize::SizeType>().value_or(-1));
						attributes.Add(FileAttribute::Compressed, fileItem.GetSize() != fileItem.GetCompressedSize());
					}
					else
					{
						return {};
					}
				}

				// Creation time
				if (FAILED(archive.GetProperty(fileIndex, kpidCTime, &property)))
				{
					return {};
				}
				fileItem.SetCreationTime(property.ToDateTime().value_or(DateTime()));

				// Modification time
				if (FAILED(archive.GetProperty(fileIndex, kpidMTime, &property)))
				{
					return {};
				}
				fileItem.SetModificationTime(property.ToDateTime().value_or(DateTime()));

				// Last access time
				if (FAILED(archive.GetProperty(fileIndex, kpidATime, &property)))
				{
					return {};
				}
				fileItem.SetLastAccessTime(property.ToDateTime().value_or(DateTime()));

				fileItem.SetAttributes(attributes);
				fileItem.SetUniqueID(LocallyUniqueID(fileIndex));
				return fileItem;
			}
			return {};
		}(const_cast<IInArchive&>(archive));
	}
	bool GetArchiveItems(wxInputStream& stream, CompressionFormat format, std::vector<FileItem>& items, EvtHandler* evtHandler)
	{
		if (stream.IsOk())
		{
			auto archive = GetArchiveReader(format);
			Utility::CallAtScopeExit atExit([&]()
			{
				archive->Close();
			});
			auto openCallback = COM::CreateLocalInstance<Callback::OpenArchive>(evtHandler);
			auto streamWrapper = COM::CreateLocalInstance<InStreamWrapper_wxInputStream>(stream, evtHandler);

			if (FAILED(archive->Open(streamWrapper, nullptr, openCallback)))
			{
				return false;
			}

			UInt32 itemCount32 = 0;
			if (archive->GetNumberOfItems(&itemCount32) != S_OK)
			{
				return false;
			}
			items.reserve(itemCount32);

			for (size_t i = 0; i < itemCount32; i++)
			{
				if (auto fileItem = GetArchiveItem(*archive, i))
				{
					// Add the item
					items.emplace_back(std::move(fileItem));
				}
			}
			return true;
		}
		return false;
	}

	CompressionFormat IdentifyCompressionFormat(wxInputStream& stream, const FSPath& path, EvtHandler* evtHandler)
	{
		if (!stream.IsOk())
		{
			return CompressionFormat::Unknown;
		}

		CompressionFormat availableFormats[] =
		{
			GetFormatByExtension(path.GetExtension()),
			CompressionFormat::SevenZip,
			CompressionFormat::Zip,
			CompressionFormat::Rar,
			CompressionFormat::Rar5,
			CompressionFormat::Iso,
			CompressionFormat::Cab,
			CompressionFormat::Tar,
			CompressionFormat::GZip,
			CompressionFormat::BZip2,
			CompressionFormat::Lzma,
			CompressionFormat::Lzma86,
		};

		WithEvtHandler eventHandler(evtHandler);
		if (eventHandler)
		{
			ArchiveEvent event = eventHandler.CreateEvent();
			event.SetProgress(0, std::size(availableFormats));
			event.SetString(wxS("Trying to identify archive compression format"));

			if (!eventHandler.SendEvent(event, ArchiveEvent::EvtIdentifyFormat))
			{
				return CompressionFormat::Unknown;
			}
		}

		// Check each format for one that works
		size_t counter = 0;
		for (const CompressionFormat format: availableFormats)
		{
			if (format != CompressionFormat::Unknown)
			{
				// Skip the same format as the one added by the file extension as we already tested for it
				if (counter != 0 && format == availableFormats[0])
				{
					continue;
				}

				if (eventHandler)
				{
					ArchiveEvent event = eventHandler.CreateEvent();
					event.SetProgress(counter, std::size(availableFormats));
					event.SetString(String::Format(wxS("Trying to open archive as %1"), GetNameByFormat(format)));

					if (!eventHandler.SendEvent(event, ArchiveEvent::EvtIdentifyFormat))
					{
						return CompressionFormat::Unknown;
					}
				}

				// Rewind the stream to avoid reading from the wrong position set by previous attempt
				stream.SeekI(0, wxSeekMode::wxFromStart);

				// There is a problem that GZip files will not be detected using the above method. Workaround for now.
				if (format == CompressionFormat::GZip)
				{
					size_t itemCount = 0;
					if (GetNumberOfItems(stream, CompressionFormat::GZip, itemCount, evtHandler) && itemCount != 0)
					{
						return CompressionFormat::GZip;
					}
					continue;
				}

				auto archive = GetArchiveReader(format);
				Utility::CallAtScopeExit atExit = ([&]()
				{
					archive->Close();
				});
				auto openCallback = COM::CreateLocalInstance<Callback::OpenArchive>(evtHandler);
				auto streamWrapper = COM::CreateLocalInstance<InStreamWrapper_wxInputStream>(stream, evtHandler);

				if (archive->Open(streamWrapper, nullptr, openCallback) == S_OK)
				{
					// We know the format if we get here, so return
					return format;
				}
			}
			counter++;
		}
		return CompressionFormat::Unknown;
	}
	std::optional<wxSeekMode> MapSeekMode(int seekMode) noexcept
	{
		switch (seekMode)
		{
			case SEEK_CUR:
			{
				return wxSeekMode::wxFromCurrent;
			}
			case SEEK_SET:
			{
				return wxSeekMode::wxFromStart;
			}
			case SEEK_END:
			{
				return wxSeekMode::wxFromEnd;
			}
		};
		return {};
	}
}
