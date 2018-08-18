#include "KxStdAfx.h"
#include "KxFramework/KxArchiveEvent.h"

wxDEFINE_EVENT(KxEVT_ARCHIVE, KxArchiveEvent);
wxDEFINE_EVENT(KxEVT_ARCHIVE_PACK, KxArchiveEvent);
wxDEFINE_EVENT(KxEVT_ARCHIVE_UNPACK, KxArchiveEvent);
wxDEFINE_EVENT(KxEVT_ARCHIVE_SEARCH, KxArchiveEvent);
wxDEFINE_EVENT(KxEVT_ARCHIVE_DONE, KxArchiveEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxArchiveEvent, KxFileOperationEvent);

KxArchiveEvent::KxArchiveEvent(wxEventType type, int id)
	:KxFileOperationEvent(type, id)
{
}
KxArchiveEvent::~KxArchiveEvent()
{
}
KxArchiveEvent* KxArchiveEvent::Clone() const
{
	return new KxArchiveEvent(*this);
}
