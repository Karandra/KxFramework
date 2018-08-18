#include "KxStdAfx.h"
#include "KxFramework/KxFileFinder.h"
#include "KxFramework/KxIncludeWindows.h"
#include "KxFramework/KxFileOperationEvent.h"

bool KxFileFinder::IsDirectoryEmpty(const wxString& directoryPath)
{
	KxFileFinder finder(directoryPath, "*");

	KxFileFinderItem item = finder.FindNext();
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
	KxFileFinderItem foundItem(this, fileInfo);
	if (!foundItem.IsCurrentOrParent())
	{
		return OnFound(foundItem);
	}
	return true;
}
bool KxFileFinder::OnFound(const KxFileFinderItem& foundItem)
{
	if (m_EvtHandler)
	{
		KxFileOperationEvent event(KxEVT_FILEOP_SEARCH);
		event.SetEventObject(m_EvtHandler);
		event.SetSource(m_Source);
		event.SetCurrent(foundItem.GetName());

		m_EvtHandler->ProcessEvent(event);
		return event.IsAllowed();
	}
	return true;
}
wxString KxFileFinder::Normalize(const wxString& source, bool start, bool end) const
{
	wxString out = source;
	if (start)
	{
		out.StartsWith("\\", &out);
	}
	if (end)
	{
		out.EndsWith("\\", &out);
	}
	return out;
}
wxString KxFileFinder::ConstructSearchQuery() const
{
	return !m_Filter.IsEmpty() ? m_Source + '\\' + m_Filter : m_Source;
}

KxFileFinder::KxFileFinder(const wxString& source, const wxString& filter, wxEvtHandler* eventHandler)
	:m_Source(Normalize(source, false, true)),
	m_Filter(filter.IsEmpty() ? KxFile::NullFilter : Normalize(filter, true, true)),
	m_EvtHandler(eventHandler)
{
}
KxFileFinder::~KxFileFinder()
{
	// If sequential search is not completed, close the handle here
	if (IsOK())
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
	wxString query = ConstructSearchQuery();
	HANDLE searchHandle = ::FindFirstFileW(query, &fileInfo);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		if (OnFound(fileInfo))
		{
			while (::FindNextFileW(searchHandle, &fileInfo) && OnFound(fileInfo))
			{
			}
		}
		::FindClose(searchHandle);
		return true;
	}
	return false;
}
KxFileFinderItem KxFileFinder::FindNext()
{
	if (!IsOK())
	{
		// No search handle available, begin operation.
		wxString query = ConstructSearchQuery();
		m_Handle = ::FindFirstFileW(query, &m_FindData);
		if (IsOK())
		{
			m_Canceled = false;
			return KxFileFinderItem(this, m_FindData);
		}
	}
	else
	{
		// We have handle, find next file.
		if (::FindNextFileW(m_Handle, &m_FindData))
		{
			return KxFileFinderItem(this, m_FindData);
		}
		
		::FindClose(m_Handle);
		m_Canceled = false;
		m_Handle = INVALID_HANDLE_VALUE;
	}
	return KxFileFinderItem();
}

//////////////////////////////////////////////////////////////////////////
void KxFileFinderItem::MakeNull(bool bAttribuesOnly)
{
	if (bAttribuesOnly)
	{
		m_Attributes = INVALID_FILE_ATTRIBUTES;
		m_ReparsePointAttributes = 0;
		m_CreationTime = wxDefaultDateTime;
		m_LastAccessTime = wxDefaultDateTime;
		m_ModificationTime = wxDefaultDateTime;
		m_FileSize = -1;
	}
	else
	{
		*this = KxFileFinderItem();
	}
}
void KxFileFinderItem::Set(const WIN32_FIND_DATAW& fileInfo)
{
	m_Attributes = fileInfo.dwFileAttributes;
	if (IsReparsePoint())
	{
		m_ReparsePointAttributes = fileInfo.dwReserved0;
	}

	m_FileSize = -1;
	if (IsFile())
	{
		ULARGE_INTEGER size = {0};
		size.HighPart = fileInfo.nFileSizeHigh;
		size.LowPart = fileInfo.nFileSizeLow;
		m_FileSize = (int64_t)size.QuadPart;
	}

	m_Name.reserve(ARRAYSIZE(fileInfo.cFileName));
	m_Name = fileInfo.cFileName;

	SetTime(fileInfo.ftCreationTime, m_CreationTime);
	SetTime(fileInfo.ftLastAccessTime, m_LastAccessTime);
	SetTime(fileInfo.ftLastWriteTime, m_ModificationTime);
}
void KxFileFinderItem::SetTime(const FILETIME& fileTime, wxDateTime& wxTimeValue) const
{
	if (fileTime.dwHighDateTime != 0 && fileTime.dwLowDateTime != 0)
	{
		SYSTEMTIME systemTime = {0};
		::FileTimeToSystemTime(&fileTime, &systemTime);
		wxTimeValue.SetFromMSWSysTime(systemTime);
	}
}

KxFileFinderItem::KxFileFinderItem(const wxString& fullPath)
{
	m_Source = fullPath.BeforeLast('\\', &m_Name);
	UpdateInfo();
}
KxFileFinderItem::KxFileFinderItem(KxFileFinder* finder, const WIN32_FIND_DATAW& fileInfo)
	:m_Source(finder->GetSource())
{
	Set(fileInfo);
}
KxFileFinderItem::~KxFileFinderItem()
{
}

bool KxFileFinderItem::IsCurrentOrParent() const
{
	return m_Name == wxT("..") || m_Name == wxT(".");
}
bool KxFileFinderItem::UpdateInfo()
{
	wxString query = m_Source + '\\' + m_Name;

	WIN32_FIND_DATAW info = {0};
	HANDLE searchHandle = ::FindFirstFileW(query, &info);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		Set(info);
		::FindClose(searchHandle);
		return true;
	}
	else
	{
		MakeNull(true);
		return false;
	}
}
