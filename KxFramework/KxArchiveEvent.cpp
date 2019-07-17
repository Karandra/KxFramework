#include "KxStdAfx.h"
#include "KxFramework/KxArchiveEvent.h"

KxEVENT_DEFINE_GLOBAL(ARCHIVE, KxArchiveEvent);
KxEVENT_DEFINE_GLOBAL(ARCHIVE_PACK, KxArchiveEvent);
KxEVENT_DEFINE_GLOBAL(ARCHIVE_UNPACK, KxArchiveEvent);
KxEVENT_DEFINE_GLOBAL(ARCHIVE_SEARCH, KxArchiveEvent);
KxEVENT_DEFINE_GLOBAL(ARCHIVE_DONE, KxArchiveEvent);

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
