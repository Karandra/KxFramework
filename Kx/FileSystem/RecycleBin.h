#pragma once
#include "Common.h"
#include "FSPath.h"
#include "FileItem.h"
#include "LegacyVolume.h"
#include "Kx/General/BinarySize.h"

namespace KxFramework
{
	class IFileSystem;

	enum class FSRecycleBinOpFlag
	{
		None = 0,

		Recursive = 1 << 0,
		LimitToFiles = 1 << 1
	};
}

namespace KxEnumClassOperations
{
	KxImplementEnum(KxFramework::FSRecycleBinOpFlag);
}

namespace KxFramework
{
	class RecycleBin final
	{
		private:
			wxChar m_Path[4] = {};
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
			bool ClearItems(FSRecycleBinOpFlag flags = FSRecycleBinOpFlag::None);

			FileItem GetItem(const FSPath& path) const;
			size_t EnumItems(std::function<bool(FileItem)> func) const;

			bool Recycle(const FSPath& path, FSRecycleBinOpFlag flags = FSRecycleBinOpFlag::None);
			bool Restore(const FSPath& path, FSRecycleBinOpFlag flags = FSRecycleBinOpFlag::None);
	};
}
