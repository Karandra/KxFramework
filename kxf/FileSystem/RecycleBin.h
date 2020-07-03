#pragma once
#include "Common.h"
#include "FSPath.h"
#include "FileItem.h"
#include "LegacyVolume.h"
#include "kxf/General/BinarySize.h"

namespace kxf
{
	class IFileSystem;

	enum class FSRecycleBinOpFlag: uint32_t
	{
		None = 0,

		Recursive = 1 << 0,
		LimitToFiles = 1 << 1
	};
	KxDeclareFlagSet(FSRecycleBinOpFlag);
}

namespace kxf
{
	class RecycleBin final
	{
		private:
			XChar m_Path[4] = {};
			LegacyVolume m_Volume;
			IFileSystem* m_FileSystem = nullptr;
			wxWindow* m_Window = nullptr;

		public:
			RecycleBin(LegacyVolume volume = {});
			RecycleBin(LegacyVolume volume, IFileSystem& fileSystem);

		public:
			bool IsEnabled() const;
			void SetWindow(wxWindow* window);

			BinarySize GetSize() const;
			size_t GetItemCount() const;
			bool ClearItems(FlagSet<FSRecycleBinOpFlag> flags = {});

			FileItem GetItem(const FSPath& path) const;
			size_t EnumItems(std::function<bool(FileItem)> func) const;

			bool Recycle(const FSPath& path, FlagSet<FSRecycleBinOpFlag> flags = {});
			bool Restore(const FSPath& path, FlagSet<FSRecycleBinOpFlag> flags = {});
	};
}
