#include "KxStdAfx.h"
#include "KxFramework/KxFileFinder.h"
#include "KxFramework/KxFile.h"
#include "KxFramework/KxIncludeWindows.h"
#include "KxFramework/KxFileOperationEvent.h"

namespace
{
	HANDLE CallFindFirstFile(const wxString& query, WIN32_FIND_DATAW& fileInfo, bool isCaseSensitive)
	{
		const DWORD searchFlags = FIND_FIRST_EX_LARGE_FETCH|(isCaseSensitive ? FIND_FIRST_EX_CASE_SENSITIVE : 0);
		return ::FindFirstFileExW(query, FindExInfoBasic, &fileInfo, FindExSearchNameMatch, NULL, searchFlags);
	}
	bool CallFindNextFile(HANDLE handle, WIN32_FIND_DATAW& fileInfo)
	{
		return ::FindNextFileW(handle, &fileInfo);
	}
	bool CallFindClose(HANDLE handle)
	{
		return ::FindClose(handle);
	}
}

bool KxFileFinder::IsDirectoryEmpty(const wxString& directoryPath)
{
	KxFileFinder finder(directoryPath, "*");

	KxFileItem item = finder.FindNext();
	if (!item.IsOK())
	{
		// No files at all, folder is empty.
		return true;
	}
	else if (item.IsCurrentOrParent())
	{
		// If first and second item are references to current and parent folders
		// and no more files found, then this particular folder is empty.
		item = finder.FindNext();
		if (item.IsCurrentOrParent())
		{
			return !finder.FindNext().IsOK();
		}
	}
	return false;
}

bool KxFileFinder::OnFound(const WIN32_FIND_DATAW& fileInfo)
{
	KxFileItem foundItem(this, fileInfo);
	if (!foundItem.IsCurrentOrParent())
	{
		return OnFound(foundItem);
	}
	return true;
}
bool KxFileFinder::OnFound(const KxFileItem& foundItem)
{
	if (m_EvtHandler)
	{
		KxFileOperationEvent event(KxEVT_FILEOP_SEARCH);
		event.SetEventObject(m_EvtHandler);
		event.SetCurrent(foundItem.GetName());

		m_EvtHandler->ProcessEvent(event);
		return event.IsAllowed();
	}
	return true;
}

KxFileFinder::KxFileFinder(const wxString& searchQuery, wxEvtHandler* eventHandler)
	:m_SearchQuery(Normalize(searchQuery, true, true))
{
}
KxFileFinder::KxFileFinder(const wxString& source, const wxString& filter, wxEvtHandler* eventHandler)
	:m_EvtHandler(eventHandler), m_SearchQuery(Normalize(ConstructSearchQuery(source, filter), true, true))
{
}
KxFileFinder::~KxFileFinder()
{
	// If sequential search is not completed, close the handle here
	if (m_Handle != INVALID_HANDLE_VALUE)
	{
		::FindClose(m_Handle);
	}
}

bool KxFileFinder::IsOK() const
{
	return m_Handle != INVALID_HANDLE_VALUE && m_Handle != NULL;
}
bool KxFileFinder::Run()
{
	WIN32_FIND_DATAW fileInfo = {0};
	HANDLE searchHandle = CallFindFirstFile(m_SearchQuery, fileInfo, m_CaseSensitive);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		if (OnFound(fileInfo))
		{
			while (CallFindNextFile(searchHandle, fileInfo) && OnFound(fileInfo))
			{
			}
		}
		CallFindClose(searchHandle);
		return true;
	}
	return false;
}
KxFileItem KxFileFinder::FindNext()
{
	if (m_Handle == INVALID_HANDLE_VALUE)
	{
		// No search handle available, begin operation.
		m_Handle = CallFindFirstFile(m_SearchQuery, m_FindData, m_CaseSensitive);
		if (m_Handle != INVALID_HANDLE_VALUE)
		{
			m_IsCanceled = false;
			return KxFileItem(this, m_FindData);
		}
	}
	else
	{
		// We have handle, find next file.
		if (CallFindNextFile(m_Handle, m_FindData))
		{
			return KxFileItem(this, m_FindData);
		}
		
		// No files left, close search handle
		CallFindClose(m_Handle);
		m_Handle = INVALID_HANDLE_VALUE;
		m_IsCanceled = false;
	}
	return KxFileItem();
}
