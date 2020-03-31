#pragma once
#include "Common.h"
#include "Kx/FileSystem/FSPath.h"

namespace KxFramework
{
	enum class SHOperationType
	{
		Copy,
		Move,
		Rename,
		Delete
	};
	enum class SHOperationFlags
	{
		None = 0,
		Recursive = 1 << 1,
		LimitToFiles = 1 << 2,
		AllowUndo = 1 << 3,
		NoConfirmation = 1 << 4,
	};
}

namespace KxEnumClassOperations
{
	KxImplementEnum(KxFramework::SHOperationFlags);
}

namespace KxFramework::Shell
{
	bool FileOperation(SHOperationType opType, const FSPath& source, const FSPath& destination, wxWindow* window = nullptr, SHOperationFlags flags = SHOperationFlags::None);
}
