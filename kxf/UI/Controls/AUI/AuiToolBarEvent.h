#pragma once
#include "kxf/UI/Common.h"
#include "kxf/EventSystem/Event.h"
#include <wx/aui/auibar.h>

namespace kxf::UI
{
	class AuiToolBarItem;
}

namespace kxf::UI
{
	class KX_API AuiToolBarEvent: public wxAuiToolBarEvent
	{
		public:
			KxEVENT_MEMBER(AuiToolBarEvent, ItemClick);
			KxEVENT_MEMBER(AuiToolBarEvent, ItemRightClick);
			KxEVENT_MEMBER(AuiToolBarEvent, ItemMiddleClick);
			KxEVENT_MEMBER(AuiToolBarEvent, ItemDropdown);
			KxEVENT_MEMBER(AuiToolBarEvent, OverflowClick);

		private:
			AuiToolBarItem* m_Item = nullptr;

		public:
			AuiToolBarEvent(wxEventType type = wxEVT_NULL, wxWindowID id = 0)
				:wxAuiToolBarEvent(type, id)
			{
			}
			AuiToolBarEvent(const wxAuiToolBarEvent& other)
			{
				SetId(other.GetId());
				SetInt(other.GetInt());
				SetExtraLong(other.GetExtraLong());
				SetString(other.GetString());
				SetEventType(other.GetEventType());

				SetDropDownClicked(other.IsDropDownClicked());
				SetClickPoint(other.GetClickPoint());
				SetItemRect(other.GetItemRect());
				SetToolId(other.GetToolId());
			}

		public:
			AuiToolBarEvent* Clone() const override
			{
				return new AuiToolBarEvent(*this);
			}

			AuiToolBarItem* GetItem() const
			{
				return m_Item;
			}
			void SetItem(AuiToolBarItem* item)
			{
				m_Item = item;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(AuiToolBarEvent);
	};
}

