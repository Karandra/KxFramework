#include "KxStdAfx.h"
#include "KxFramework/KxFileOperationEvent.h"

KxEVENT_DEFINE_GLOBAL(FILEOP_COPY, KxFileOperationEvent);
KxEVENT_DEFINE_GLOBAL(FILEOP_MOVE, KxFileOperationEvent);
KxEVENT_DEFINE_GLOBAL(FILEOP_REMOVE, KxFileOperationEvent);
KxEVENT_DEFINE_GLOBAL(FILEOP_COPY_FOLDER, KxFileOperationEvent);
KxEVENT_DEFINE_GLOBAL(FILEOP_MOVE_FOLDER, KxFileOperationEvent);
KxEVENT_DEFINE_GLOBAL(FILEOP_REMOVE_FOLDER, KxFileOperationEvent);
KxEVENT_DEFINE_GLOBAL(FILEOP_SEARCH, KxFileOperationEvent);
KxEVENT_DEFINE_GLOBAL(FILEOP_RENAME, KxFileOperationEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxFileOperationEvent, wxNotifyEvent);

KxFileOperationEvent::KxFileOperationEvent(wxEventType type, int id)
	:wxNotifyEvent(type, id)
{
	Allow();
}
KxFileOperationEvent::~KxFileOperationEvent()
{
}
KxFileOperationEvent* KxFileOperationEvent::Clone() const
{
	return new KxFileOperationEvent(*this);
}
