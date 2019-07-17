#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/aui/auibar.h>
#include "KxEvent.h"
class KX_API KxAuiToolBarItem;

class KX_API KxAuiToolBarEvent: public wxAuiToolBarEvent
{
	private:
		KxAuiToolBarItem* m_Item = nullptr;

	public:
		KxAuiToolBarEvent(const wxAuiToolBarEvent& other);
		KxAuiToolBarEvent(wxEventType type = wxEVT_NULL, wxWindowID id = 0);
		~KxAuiToolBarEvent();

	public:
		KxAuiToolBarEvent* Clone() const override;

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

KxEVENT_DECLARE_GLOBAL(AUI_TOOLBAR_CLICK, KxAuiToolBarEvent);
KxEVENT_DECLARE_GLOBAL(AUI_TOOLBAR_RIGHT_CLICK, KxAuiToolBarEvent);
KxEVENT_DECLARE_GLOBAL(AUI_TOOLBAR_MIDDLE_CLICK, KxAuiToolBarEvent);
KxEVENT_DECLARE_GLOBAL(AUI_TOOLBAR_DROPDOWN, KxAuiToolBarEvent);
KxEVENT_DECLARE_GLOBAL(AUI_TOOLBAR_OVERFLOW, KxAuiToolBarEvent);
