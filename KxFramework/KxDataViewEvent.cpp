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
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_SELECTED, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_HOVERED, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_ACTIVATED, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_COLLAPSED, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_EXPANDED, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_COLLAPSING, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_EXPANDING, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_CONTEXT_MENU, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_VALUE_CHANGED, KxDataViewEvent);

KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_EDIT_ATTACH, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_EDIT_DETACH, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_EDIT_STARTING, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_EDIT_STARTED, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_EDIT_DONE, KxDataViewEvent);

KxEVENT_DEFINE_GLOBAL(DATAVIEW_COLUMN_HEADER_CLICK, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_COLUMN_SORTED, KxDataViewEvent);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_COLUMN_REORDERED, KxDataViewEvent);

KxEVENT_DEFINE_GLOBAL(DATAVIEW_CACHE_HINT, KxDataViewEvent);

KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_DRAG, KxDataViewEventDND);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_DROP_POSSIBLE, KxDataViewEventDND);
KxEVENT_DEFINE_GLOBAL(DATAVIEW_ITEM_DROP, KxDataViewEventDND);
