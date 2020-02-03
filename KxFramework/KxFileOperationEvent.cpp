#include "KxStdAfx.h"
#include "KxFramework/KxFileOperationEvent.h"

KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_COPY);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_MOVE);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_REMOVE);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_COPY_FOLDER);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_MOVE_FOLDER);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_REMOVE_FOLDER);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_SEARCH);
KxEVENT_DEFINE_GLOBAL(KxFileOperationEvent, FILEOP_RENAME);

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
