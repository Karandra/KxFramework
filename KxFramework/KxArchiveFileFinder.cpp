#include "KxStdAfx.h"
#include "KxFramework/KxArchiveFileFinder.h"
#include "KxFramework/KxArchiveEvent.h"
#include "KxFramework/KxIArchive.h"
#include "KxFramework/KxFile.h"

bool KxArchiveFileFinder::OnFound(const KxFileItem& foundItem)
{
	if (m_EvtHandler)
	{
		KxArchiveEvent event(KxEVT_FILEOP_SEARCH);
		event.SetEventObject(m_EvtHandler);
		event.SetSource(foundItem.GetSource());
		event.SetCurrent(foundItem.GetName());

		m_EvtHandler->ProcessEvent(event);
		return event.IsAllowed();
	}
	return true;
}

KxArchiveFileFinder::KxArchiveFileFinder(const KxIArchiveSearch& archive, const wxString& searchQuery, wxEvtHandler* eventHandler)
	:m_Archive(&archive),
	m_SearchQuery(Normalize(searchQuery, false, true)),
	m_EvtHandler(eventHandler)
{
}
KxArchiveFileFinder::KxArchiveFileFinder(const KxIArchiveSearch& archive, const wxString& source, const wxString& filter, wxEvtHandler* eventHandler)
	:m_Archive(&archive),
	m_SearchQuery(Normalize(ConstructSearchQuery(source, filter), false, true)),
	m_EvtHandler(eventHandler)
{
}
KxArchiveFileFinder::~KxArchiveFileFinder()
{
	if (m_Archive && m_Handle)
	{
		m_Archive->FindClose(m_Handle);
	}
}

bool KxArchiveFileFinder::IsOK() const
{
	return m_Archive != NULL && m_Handle != NULL;
}
bool KxArchiveFileFinder::Run()
{
	KxFileItem fileItem;
	void* searchHandle = m_Archive->FindFirstFile(m_SearchQuery, fileItem);
	if (searchHandle)
	{
		if (OnFound(fileItem))
		{
			while (m_Archive->FindNextFile(searchHandle, fileItem) && OnFound(fileItem))
			{
			}
		}
		m_Archive->FindClose(searchHandle);
		return true;
	}
	return false;
}
KxFileItem KxArchiveFileFinder::FindNext()
{
	if (m_Handle == NULL)
	{
		// No search handle available, begin operation.
		KxFileItem fileItem;
		m_Handle = m_Archive->FindFirstFile(m_SearchQuery, fileItem);
		if (m_Handle)
		{
			m_IsCanceled = false;
			return fileItem;
		}
	}
	else
	{
		// We have handle, find next file.
		KxFileItem fileItem;
		if (m_Archive->FindNextFile(m_Handle, fileItem))
		{
			return fileItem;
		}
		
		// No files left, close search handle
		m_Archive->FindClose(m_Handle);
		m_IsCanceled = false;
		m_Handle = NULL;
	}
	return KxFileItem();
}
