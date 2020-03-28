#pragma once
#include "Common.h"

namespace KxFramework
{
	class FileItem;
}

namespace KxFramework
{
	class KX_API IFileSearcher
	{
		protected:
			virtual bool OnItemFound(const FileItem& foundItem) = 0;

		public:
			virtual ~IFileSearcher() = default;

		public:
			virtual bool Run() = 0;
			virtual bool WasCanceled() const = 0;

			virtual FileItem FindNext() = 0;
	};
}
