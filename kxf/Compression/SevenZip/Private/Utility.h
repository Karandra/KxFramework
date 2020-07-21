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
	bool GetNumberOfItems(wxInputStream& stream, CompressionFormat format, size_t& itemCount, EvtHandler* evtHandler = nullptr);

	FileItem GetArchiveItem(const IInArchive& archive, size_t fileIndex);
	bool GetArchiveItems(wxInputStream& stream, CompressionFormat format, std::vector<FileItem>& items, EvtHandler* evtHandler = nullptr);

	CompressionFormat IdentifyCompressionFormat(wxInputStream& stream, const FSPath& path = {}, EvtHandler* evtHandler = nullptr);
	std::optional<wxSeekMode> MapSeekMode(int seekMode) noexcept;;
}
