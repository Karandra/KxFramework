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
	KxEVENT_DEFINE_LOCAL(ITEM_SELECTED, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_HOVERED, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_ACTIVATED, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_COLLAPSED, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_EXPANDED, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_COLLAPSING, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_EXPANDING, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_CONTEXT_MENU, Event);
	KxEVENT_DEFINE_LOCAL(ITEM_VALUE_CHANGED, Event);

	KxEVENT_DEFINE_LOCAL(ITEM_EDIT_STARTING, EventEditor);
	KxEVENT_DEFINE_LOCAL(ITEM_EDIT_STARTED, EventEditor);
	KxEVENT_DEFINE_LOCAL(ITEM_EDIT_DONE, EventEditor);

	KxEVENT_DEFINE_LOCAL(ITEM_DRAG, EventDND);
	KxEVENT_DEFINE_LOCAL(ITEM_DROP, EventDND);
	KxEVENT_DEFINE_LOCAL(ITEM_DROP_POSSIBLE, EventDND);

	KxEVENT_DEFINE_LOCAL(COLUMN_HEADER_CLICK, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_HEADER_RCLICK, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_HEADER_SEPARATOR_CLICK, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_DROPDOWN, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_TOGGLE, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_SORTED, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_MOVED, Event);

	KxEVENT_DEFINE_LOCAL(COLUMN_BEGIN_RESIZE, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_RESIZE, Event);
	KxEVENT_DEFINE_LOCAL(COLUMN_END_RESIZE, Event);

	KxEVENT_DEFINE_LOCAL(VIEW_CACHE_HINT, Event);
}
