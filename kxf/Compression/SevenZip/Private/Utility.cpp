#include "stdafx.h"
#include "Utility.h"
#include "GUIDs.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include "../Library.h"
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
	COMPtr<IStream> OpenFileToRW(const FSPath& filePath)
	{
		bool fileExists = NativeFileSystem::Get().GetItem(filePath).IsValid();
		return CreateStreamOnFile(filePath, (fileExists ? 0 : STGM_CREATE)|STGM_READWRITE);
	}
	COMPtr<IStream> OpenFileToRead(const FSPath& filePath)
	{
		return CreateStreamOnFile(filePath, STGM_READ);
	}
	COMPtr<IStream> OpenFileToWrite(const FSPath& filePath)
	{
		return CreateStreamOnFile(filePath, STGM_CREATE|STGM_WRITE);
	}

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

	std::optional<size_t> GetNumberOfItems(const COMPtr<IInArchive>& archive) noexcept
	{
		uint32_t itemCount = 0;
		if (archive->GetNumberOfItems(&itemCount) == S_OK)
		{
			// If there are items try to verify if they're readable
			if (itemCount != 0)
			{
				VariantProperty property;
				if (HResult(archive->GetProperty(0, kpidSize, &property)) && !property.IsEmpty())
				{
					return itemCount;
				}
			}
		}
		return {};
	}
	bool GetNumberOfItems(const FSPath& archivePath, CompressionFormat format, size_t& itemCount, wxEvtHandler* evtHandler)
	{
		/*
		if (auto fileStream = OpenFileToRead(archivePath))
		{
			auto archive = GetArchiveReader(format);
			auto inFile = Library::GetInstance().CreateObject<InStreamWrapper>(fileStream, notifier);
			auto openCallback = Library::GetInstance().CreateObject<Callback::OpenArchive>(archivePath, notifier);

			if (HResult(archive->Open(inFile, nullptr, openCallback)))
			{
				Utility::CallAtScopeExit atExit([&]()
				{
					archive->Close();
					if (evtHandler)
					{
						evtHandler->OnEnd();
					}
				});

				if (evtHandler)
				{
					evtHandler->OnStart(_T("Getting number of items"), 0);
				}
				if (auto count = GetNumberOfItems(archive))
				{
					itemCount = *count;
					return true;
				}
			}
		}
		*/
		return false;
	}
}
