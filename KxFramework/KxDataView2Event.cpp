#include "stdafx.h"
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
	KxEVENT_DEFINE_LOCAL(Event, ITEM_SELECTED);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_HOVERED);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_ACTIVATED);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_COLLAPSED);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_EXPANDED);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_COLLAPSING);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_EXPANDING);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_CONTEXT_MENU);
	KxEVENT_DEFINE_LOCAL(Event, ITEM_VALUE_CHANGED);

	KxEVENT_DEFINE_LOCAL(EventEditor, ITEM_EDIT_STARTING);
	KxEVENT_DEFINE_LOCAL(EventEditor, ITEM_EDIT_STARTED);
	KxEVENT_DEFINE_LOCAL(EventEditor, ITEM_EDIT_DONE);

	KxEVENT_DEFINE_LOCAL(EventDND, ITEM_DRAG);
	KxEVENT_DEFINE_LOCAL(EventDND, ITEM_DROP);
	KxEVENT_DEFINE_LOCAL(EventDND, ITEM_DROP_POSSIBLE);

	KxEVENT_DEFINE_LOCAL(Event, COLUMN_HEADER_CLICK);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_HEADER_RCLICK);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_HEADER_MENU_ITEM);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_HEADER_SEPARATOR_DCLICK);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_HEADER_WIDTH_FIT);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_DROPDOWN);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_TOGGLE);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_SORTED);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_MOVED);

	KxEVENT_DEFINE_LOCAL(Event, COLUMN_BEGIN_RESIZE);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_RESIZE);
	KxEVENT_DEFINE_LOCAL(Event, COLUMN_END_RESIZE);

	KxEVENT_DEFINE_LOCAL(Event, VIEW_CACHE_HINT);
}
