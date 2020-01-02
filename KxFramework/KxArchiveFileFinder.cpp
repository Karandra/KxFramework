#include "KxStdAfx.h"
#include "KxFramework/KxArchiveFileFinder.h"
#include "KxFramework/KxArchiveEvent.h"
#include "KxFramework/KxIArchive.h"
#include "KxFramework/KxFile.h"

namespace KxArchive
{
	bool FileFinder::OnFound(const KxFileItem& foundItem)
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

	FileFinder::FileFinder(const IArchiveSearch& archive, const wxString& searchQuery, wxEvtHandler* eventHandler)
		:m_Archive(&archive),
		m_SearchQuery(Normalize(searchQuery, false, true)),
		m_EvtHandler(eventHandler)
	{
	}
	FileFinder::FileFinder(const IArchiveSearch& archive, const wxString& source, const wxString& filter, wxEvtHandler* eventHandler)
		: m_Archive(&archive),
		m_SearchQuery(Normalize(ConstructSearchQuery(source, filter), false, true)),
		m_EvtHandler(eventHandler)
	{
	}
	FileFinder::~FileFinder()
	{
		if (m_Archive && m_Handle)
		{
			m_Archive->FindClose(m_Handle);
		}
	}

	bool FileFinder::IsOK() const
	{
		return m_Archive != nullptr && m_Handle != nullptr;
	}
	bool FileFinder::Run()
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
	KxFileItem FileFinder::FindNext()
	{
		if (m_Handle == nullptr)
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
			m_Handle = nullptr;
		}
		return {};
	}
}
