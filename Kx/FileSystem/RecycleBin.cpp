#include "KxStdAfx.h"
#include "RecycleBin.h"
#include "NativeFileSystem.h"
#include <shellapi.h>
#include "Kx/General/UndefWindows.h"
#include "Kx/System/ShellOperations.h"
#include "Kx/Utility/Common.h"

namespace
{
	// https://www.codeproject.com/Articles/2783/How-to-programmatically-use-the-Recycle-Bin
	// https://oipapio.com/question-589504

	std::optional<SHQUERYRBINFO> QueryRecycleBin(const wxChar* path)
	{
		SHQUERYRBINFO queryInfo = {};
		queryInfo.cbSize = sizeof(queryInfo);

		if (::SHQueryRecycleBinW(path, &queryInfo) == S_OK)
		{
			return queryInfo;
		}
		return {};
	}
}

namespace KxFramework
{
	RecycleBin::RecycleBin(LegacyVolume volume)
		:RecycleBin(volume, NativeFileSystem::GetInstance())
	{
	}
	RecycleBin::RecycleBin(LegacyVolume volume, IFileSystem& fileSystem)
		:m_Volume(volume), m_FileSystem(&fileSystem)
	{
		if (volume)
		{
			m_Path[0] = volume.GetChar();
			m_Path[1] = wxS(':');
			m_Path[2] = wxS('\\');
			m_Path[3] = wxS('\0');
		}
	}

	bool RecycleBin::IsEnabled() const
	{
		return m_Volume && !GetSize().IsValid();
	}

	BinarySize RecycleBin::GetSize() const
	{
		if (auto queryInfo = QueryRecycleBin(m_Path))
		{
			return BinarySize::FromBytes(queryInfo->i64Size);
		}
		return {};
	}
	size_t RecycleBin::GetItemCount() const
	{
		if (auto queryInfo = QueryRecycleBin(m_Path))
		{
			return queryInfo->i64NumItems;
		}
		return 0;
	}

	FileItem RecycleBin::GetItem(const FSPath& path) const
	{
		throw std::logic_error(__FUNCTION__ ": the method or operation is not implemented.");
	}
	size_t RecycleBin::EnumItems(std::function<bool(FileItem)> func) const
	{
		throw std::logic_error(__FUNCTION__ ": the method or operation is not implemented.");
	}
	
	bool RecycleBin::Recycle(const FSPath& path, FSRecycleItemFlag flags)
	{
		if (path.ContainsCharacters(wxS("*?")))
		{
			if (flags & FSRecycleItemFlag::Recursive)
			{
				SHOperationFlags shellFlags = SHOperationFlags::AllowUndo|SHOperationFlags::Recursive;
				Utility::ModFlagRef(shellFlags, SHOperationFlags::LimitToFiles, flags & FSRecycleItemFlag::LimitToFiles);

				return Shell::FileOperation(SHOperationType::Delete, path, {}, m_Window, shellFlags);
			}
			return false;
		}
		else
		{
			if (FileItem fileItem = m_FileSystem->GetItem(path))
			{
				if (fileItem.IsDirectory())
				{
					SHOperationFlags shellFlags = SHOperationFlags::AllowUndo;
					Utility::ModFlagRef(shellFlags, SHOperationFlags::Recursive, flags & FSRecycleItemFlag::Recursive);

					return Shell::FileOperation(SHOperationType::Delete, path, {}, m_Window, shellFlags);
				}
				else
				{
					return Shell::FileOperation(SHOperationType::Delete, path, {}, m_Window, SHOperationFlags::AllowUndo|SHOperationFlags::LimitToFiles);
				}
			}
		}
		return false;
	}
	bool RecycleBin::Restore(const FSPath& path, FSRecycleItemFlag flags)
	{
		throw std::logic_error(__FUNCTION__ ": the method or operation is not implemented.");
	}
}
