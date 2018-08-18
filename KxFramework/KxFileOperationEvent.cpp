#include "KxStdAfx.h"
#include "KxFramework/KxFileOperationEvent.h"

wxDEFINE_EVENT(KxEVT_FILEOP_COPY, KxFileOperationEvent);
wxDEFINE_EVENT(KxEVT_FILEOP_MOVE, KxFileOperationEvent);
wxDEFINE_EVENT(KxEVT_FILEOP_REMOVE, KxFileOperationEvent);
wxDEFINE_EVENT(KxEVT_FILEOP_COPY_FOLDER, KxFileOperationEvent);
wxDEFINE_EVENT(KxEVT_FILEOP_MOVE_FOLDER, KxFileOperationEvent);
wxDEFINE_EVENT(KxEVT_FILEOP_REMOVE_FOLDER, KxFileOperationEvent);
wxDEFINE_EVENT(KxEVT_FILEOP_SEARCH, KxFileOperationEvent);
wxDEFINE_EVENT(KxEVT_FILEOP_RENAME, KxFileOperationEvent);

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
