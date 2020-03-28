#pragma once
#include "Common.h"
#include "FSPath.h"

namespace KxFramework
{
	class FileItem;

	enum class FSEnumItemsFlag
	{
		None = 0,

		CaseSensitive = 1 << 0,
		Recursive = 1 << 1,
	};
}

namespace KxEnumClassOperations
{
	KxImplementEnum(KxFramework::FSEnumItemsFlag);
}

namespace KxFramework
{
	class KX_API IFileSystem
	{
		public:
			virtual ~IFileSystem() = default;

		public:
			virtual FileItem GetItem(const FSPath& path) const = 0;
			virtual size_t EnumItems(const FSPath& directory, std::function<bool(const FileItem&)> func, const wxString& query = {}, FSEnumItemsFlag flags = FSEnumItemsFlag::None) const = 0;
	};
}
