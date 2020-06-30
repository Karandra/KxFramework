#pragma once
#include "../Common.h"
#include "kxf/System/COM.h"
#include "kxf/System/VariantProperty.h"
#include <7zip/CPP/7zip/Archive/IArchive.h>
#include <7zip/C/7zTypes.h>

namespace kxf::SevenZip::Private
{
	template<class T, class... Args>
	COMPtr<T> CreateObject(Args&&... arg) noexcept(std::is_nothrow_constructible_v<T, Args...>)
	{
		return std::make_unique<T>(std::forward<Args>(arg)...).release();
	}

	COMPtr<IStream> OpenFileToRW(const FSPath& filePath);
	COMPtr<IStream> OpenFileToRead(const FSPath& filePath);
	COMPtr<IStream> OpenFileToWrite(const FSPath& filePath);

	COMPtr<IInArchive> GetArchiveReader(CompressionFormat format) noexcept;
	COMPtr<IOutArchive> GetArchiveWriter(CompressionFormat format) noexcept;

	std::optional<size_t> GetNumberOfItems(const COMPtr<IInArchive>& archive) noexcept;
	bool GetNumberOfItems(const FSPath& archivePath, CompressionFormat format, size_t& itemCount, wxEvtHandler* evtHandler = nullptr);

	FileItem GetArchiveItem(const COMPtr<IInArchive>& archive, size_t fileIndex);
	bool GetArchiveItems(const FSPath& archivePath, CompressionFormat format, std::vector<FileItem>& items, wxEvtHandler* evtHandler = nullptr);

	CompressionFormat IdentifyCompressionFormat(const FSPath& archivePath, wxEvtHandler* evtHandler = nullptr);
	std::optional<wxSeekMode> MapSeekMode(int seekMode) noexcept;;
}
