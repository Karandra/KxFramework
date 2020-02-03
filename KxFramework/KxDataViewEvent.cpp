#include "KxStdAfx.h"
#include "KxFramework/KxDataViewEvent.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"

KxDataViewCtrl* KxDataViewEvent::GetOwner() const
{
	return static_cast<KxDataViewCtrl*>(GetEventObject());
}
KxDataViewMainWindow* KxDataViewEvent::GetMainWindow() const
{
	return GetOwner()->GetMainWindow();
}
KxDataViewListModel* KxDataViewEvent::GetListModel() const
{
	return dynamic_cast<KxDataViewListModel*>(m_Model);
}

KxDataViewEvent::KxDataViewEvent(wxEventType type, wxWindowID winid)
	:wxNotifyEvent(type, winid),
	m_CacheHintFrom(KxDataViewMainWindow::INVALID_ROW), m_CacheHintTo(KxDataViewMainWindow::INVALID_ROW)
{
}

size_t KxDataViewEvent::GetCacheHintRowFrom() const
{
	if (KxDataViewListModel* model = GetListModel())
	{
		return model->GetRow(GetCacheHintItemFrom());
	}
	return KxDataViewMainWindow::INVALID_ROW;
}
size_t KxDataViewEvent::GetCacheHintRowTo() const
{
	if (KxDataViewListModel* model = GetListModel())
	{
		return model->GetRow(GetCacheHintItemTo());
	}
	return KxDataViewMainWindow::INVALID_ROW;
}

KxDataViewItem KxDataViewEvent::GetCacheHintItemFrom() const
{
	return GetMainWindow()->GetItemByRow(m_CacheHintFrom);
}
KxDataViewItem KxDataViewEvent::GetCacheHintItemTo() const
{
	return GetMainWindow()->GetItemByRow(m_CacheHintTo);
}

//////////////////////////////////////////////////////////////////////////
KxDataViewEventDND::KxDataViewEventDND(wxEventType type, wxWindowID winid)
	:KxDataViewEvent(type, winid)
{
}

//////////////////////////////////////////////////////////////////////////
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_SELECTED);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_HOVERED);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_ACTIVATED);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_COLLAPSED);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_EXPANDED);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_COLLAPSING);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_EXPANDING);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_CONTEXT_MENU);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_VALUE_CHANGED);

KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_EDIT_ATTACH);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_EDIT_DETACH);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_EDIT_STARTING);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_EDIT_STARTED);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_ITEM_EDIT_DONE);

KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_COLUMN_HEADER_CLICK);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_COLUMN_HEADER_RIGHT_CLICK);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_COLUMN_SORTED);
KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_COLUMN_REORDERED);

KxEVENT_DEFINE_GLOBAL(KxDataViewEvent, DATAVIEW_CACHE_HINT);

KxEVENT_DEFINE_GLOBAL(KxDataViewEventDND, DATAVIEW_ITEM_DRAG);
KxEVENT_DEFINE_GLOBAL(KxDataViewEventDND, DATAVIEW_ITEM_DROP_POSSIBLE);
KxEVENT_DEFINE_GLOBAL(KxDataViewEventDND, DATAVIEW_ITEM_DROP);
