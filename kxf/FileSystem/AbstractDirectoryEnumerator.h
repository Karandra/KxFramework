#pragma once
#include "Common.h"
#include "FSPath.h"
#include "FileItem.h"

namespace kxf
{
	class IEnumerator;
}

namespace kxf
{
	class KX_API AbstractDirectoryEnumerator
	{
		private:
			FSPath m_RootPath;

			std::vector<FSPath> m_SubDirectories;
			bool m_SubTreeDone = false;

			std::vector<FSPath> m_NextSubDirectories;
			size_t m_NextSubDirectory = 0;
			bool m_NextSubTreeDone = false;

		private:
			std::optional<FileItem> MoveNext(IEnumerator& enumerator);

		protected:
			virtual std::optional<FileItem> SearchDirectory(IEnumerator& enumerator, const FSPath& directory, std::vector<FSPath>& childDirectories, bool& isSubTreeDone) = 0;

		public:
			AbstractDirectoryEnumerator() = default;
			AbstractDirectoryEnumerator(FSPath rootPath)
				:m_RootPath(std::move(rootPath))
			{
			}
			virtual ~AbstractDirectoryEnumerator() = default;

		public:
			std::optional<FileItem> operator()(IEnumerator& enumerator)
			{
				return MoveNext(enumerator);
			}
			const FSPath& GetRootPath() const
			{
				return m_RootPath;
			}
	};
}
