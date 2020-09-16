#pragma once
#include "../Common.h"
#include "kxf/System/COM.h"
#include "kxf/System/VariantProperty.h"
#include <7zip/CPP/7zip/Archive/IArchive.h>
#include <7zip/C/7zTypes.h>

namespace kxf::SevenZip::Private
{
	COMPtr<IInArchive> GetArchiveReader(CompressionFormat format) noexcept;
	COMPtr<IOutArchive> GetArchiveWriter(CompressionFormat format) noexcept;

	std::optional<size_t> GetNumberOfItems(const IInArchive& archive) noexcept;
	bool GetNumberOfItems(IInputStream& stream, CompressionFormat format, size_t& itemCount, IEvtHandler* evtHandler = nullptr);

	FileItem GetArchiveItem(const IInArchive& archive, size_t fileIndex);
	bool GetArchiveItems(IInputStream& stream, CompressionFormat format, std::vector<FileItem>& items, IEvtHandler* evtHandler = nullptr);

	CompressionFormat IdentifyCompressionFormat(IInputStream& stream, const FSPath& path = {}, IEvtHandler* evtHandler = nullptr);
	std::optional<IOStreamSeek> MapSeekMode(int seekMode) noexcept;;
}
