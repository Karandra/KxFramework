#include "KxStdAfx.h"
#include "KxFramework/KxDataView2Event.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/MainWindow.h"

namespace KxDataView2
{
	View* Event::GetView() const
	{
		return static_cast<View*>(wxNotifyEvent::GetEventObject());
	}
	MainWindow* Event::GetMainWindow() const
	{
		if (View* view = GetView())
		{
			return view->GetMainWindow();
		}
		return nullptr;
	}
}

namespace KxDataView2
{
	wxDataObjectSimple* EventDND::GetDragObject(const wxDataFormat& format) const
	{
		MainWindow* mainWindow = GetMainWindow();
		if (mainWindow && mainWindow->m_DragDropDataObject)
		{
			return mainWindow->m_DragDropDataObject->GetObject(format);
		}
		return nullptr;
	}
	void EventDND::DragDone(const wxDataObjectSimple& dataObject, int flags)
	{
		m_DataObject = const_cast<wxDataObjectSimple*>(&dataObject);
		m_DragFlags = flags;
	}
	void EventDND::DragCancel()
	{
		m_DataObject = nullptr;
		m_DragFlags = wxDrag_CopyOnly;
	}

	wxDataObjectSimple* EventDND::GetRecievedDataObject() const
	{
		return m_DataObject;
	}
	void EventDND::DropDone(wxDragResult result)
	{
		m_DropResult = result;
	}
	void EventDND::DropCancel()
	{
		m_DropResult = wxDragCancel;
	}
	void EventDND::DropError()
	{
		m_DropResult = wxDragError;
	}
}

namespace KxDataView2
{
	wxDEFINE_EVENT(EVENT_ITEM_SELECTED, Event);
	wxDEFINE_EVENT(EVENT_ITEM_HOVERED, Event);
	wxDEFINE_EVENT(EVENT_ITEM_ACTIVATED, Event);
	wxDEFINE_EVENT(EVENT_ITEM_COLLAPSED, Event);
	wxDEFINE_EVENT(EVENT_ITEM_EXPANDED, Event);
	wxDEFINE_EVENT(EVENT_ITEM_COLLAPSING, Event);
	wxDEFINE_EVENT(EVENT_ITEM_EXPANDING, Event);
	wxDEFINE_EVENT(EVENT_ITEM_CONTEXT_MENU, Event);
	wxDEFINE_EVENT(EVENT_ITEM_VALUE_CHANGED, Event);

	wxDEFINE_EVENT(EVENT_ITEM_EDIT_STARTING, EventEditor);
	wxDEFINE_EVENT(EVENT_ITEM_EDIT_STARTED, EventEditor);
	wxDEFINE_EVENT(EVENT_ITEM_EDIT_DONE, EventEditor);

	wxDEFINE_EVENT(EVENT_ITEM_DRAG, EventDND);
	wxDEFINE_EVENT(EVENT_ITEM_DROP, EventDND);
	wxDEFINE_EVENT(EVENT_ITEM_DROP_POSSIBLE, EventDND);

	wxDEFINE_EVENT(EVENT_COLUMN_HEADER_CLICK, Event);
	wxDEFINE_EVENT(EVENT_COLUMN_HEADER_RCLICK, Event);
	wxDEFINE_EVENT(EVENT_COLUMN_SORTED, Event);
	wxDEFINE_EVENT(EVENT_COLUMN_MOVED, Event);

	wxDEFINE_EVENT(EVENT_VIEW_CACHE_HINT, Event);
}
