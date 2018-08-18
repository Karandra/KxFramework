#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/aui/auibar.h>
class KxAuiToolBarItem;

class KxAuiToolBarEvent: public wxAuiToolBarEvent
{
	private:
		KxAuiToolBarItem* m_Item = NULL;

	public:
		KxAuiToolBarEvent(const wxAuiToolBarEvent& other);
		KxAuiToolBarEvent(wxEventType type = wxEVT_NULL, wxWindowID id = 0);
		virtual ~KxAuiToolBarEvent();
		virtual KxAuiToolBarEvent* Clone() const override;

	public:
		KxAuiToolBarItem* GetItem() const
		{
			return m_Item;
		}
		void SetItem(KxAuiToolBarItem* item)
		{
			m_Item = item;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxAuiToolBarEvent);
};

wxDECLARE_EVENT(KxEVT_AUI_TOOLBAR_CLICK, KxAuiToolBarEvent);
wxDECLARE_EVENT(KxEVT_AUI_TOOLBAR_RIGHT_CLICK, KxAuiToolBarEvent);
wxDECLARE_EVENT(KxEVT_AUI_TOOLBAR_MIDDLE_CLICK, KxAuiToolBarEvent);
wxDECLARE_EVENT(KxEVT_AUI_TOOLBAR_DROPDOWN, KxAuiToolBarEvent);
wxDECLARE_EVENT(KxEVT_AUI_TOOLBAR_OVERFLOW, KxAuiToolBarEvent);
