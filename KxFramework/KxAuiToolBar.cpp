#include "KxStdAfx.h"
#include "KxFramework/KxAuiToolBar.h"
#include "KxFramework/KxAuiToolBarItem.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxAuiToolBar, wxAuiToolBar);

bool KxAuiToolBar::IsValidID(wxWindowID id) const
{
	return id != wxID_NONE && id != wxID_SEPARATOR && id != wxID_ANY;
}
void KxAuiToolBar::EventHandler(wxAuiToolBarEvent& event)
{
	if (IsValidID(event.GetToolId()))
	{
		KxAuiToolBarItem* item = FindToolByID(event.GetToolId());
		if (item)
		{
			bool canShowMenu = false;
			wxEventType type = event.GetEventType();
			if (type == wxEVT_AUITOOLBAR_RIGHT_CLICK)
			{
				type = KxEVT_AUI_TOOLBAR_RIGHT_CLICK;
				canShowMenu = item->HasDropdownMenu() && item->IsOptionEnabled(KxAUI_TBITEM_OPTION_RCLICK_MENU);
			}
			else if (type == wxEVT_AUITOOLBAR_MIDDLE_CLICK)
			{
				type = KxEVT_AUI_TOOLBAR_MIDDLE_CLICK;
				canShowMenu = item->HasDropdownMenu() && item->IsOptionEnabled(KxAUI_TBITEM_OPTION_MCLICK_MENU);
			}
			else if (type == wxEVT_AUITOOLBAR_TOOL_DROPDOWN)
			{
				type = KxEVT_AUI_TOOLBAR_DROPDOWN;
			}
			else if (type == wxEVT_AUITOOLBAR_OVERFLOW_CLICK)
			{
				type = KxEVT_AUI_TOOLBAR_OVERFLOW;
			}
			else
			{
				type = wxEVT_NULL;
			}

			if (type != wxEVT_NULL)
			{
				KxAuiToolBarEvent newEvent(event);
				newEvent.SetId(GetId());
				newEvent.SetEventType(type);
				newEvent.SetEventObject(item);
				newEvent.SetInt(item->IsToggled());
				newEvent.SetItem(item);

				bool wasProcessed = item->ProcessEvent(newEvent);

				// Handle dropdown menu
				if (canShowMenu && (!wasProcessed || newEvent.GetSkipped()))
				{
					item->ShowDropdownMenu();
					item->Refresh();
					DoIdleUpdate();
				}
			}
		}
	}
	event.Skip();
}
void KxAuiToolBar::OnLeftClick(wxCommandEvent& event)
{
	if (IsValidID(event.GetId()))
	{
		KxAuiToolBarItem* item = FindToolByID(event.GetId());
		if (item)
		{
			KxAuiToolBarEvent newEvent(KxEVT_AUI_TOOLBAR_CLICK, GetId());
			newEvent.SetEventObject(item);
			newEvent.SetToolId(item->GetID());
			newEvent.SetInt(item->IsToggled());
			newEvent.SetItem(item);

			bool bProcessed = item->ProcessEvent(newEvent);

			// Handle dropdown menu
			if (item->IsOptionEnabled(KxAUI_TBITEM_OPTION_LCLICK_MENU) && item->HasDropdownMenu() && (!bProcessed || newEvent.GetSkipped()))
			{
				item->ShowDropdownMenu();
				item->Refresh();
				DoIdleUpdate();
			}
		}
	}
	event.Skip();
}

void KxAuiToolBar::Clear()
{
	for (auto& v: m_Items)
	{
		delete v.second;
	}
	m_Items.clear();
}
KxAuiToolBarItem* KxAuiToolBar::OnCreateTool(wxAuiToolBarItem* item)
{
	auto element = m_Items.emplace_back(std::make_pair(item->GetId(), new KxAuiToolBarItem(this, item)));
	return element.second;
}
KxAuiToolBarItem_ConstIterator KxAuiToolBar::GetIteratorToTool(wxWindowID id) const
{
	if (IsValidID(id))
	{
		return std::find_if(m_Items.begin(), m_Items.end(), [id](const KxAuiToolBarItem_ArrayElement& element)
		{
			return element.first == id;
		});
	}
	return m_Items.end();
}
bool KxAuiToolBar::RemoveByIterator(const KxAuiToolBarItem_ConstIterator& it)
{
	if (it != m_Items.end())
	{
		delete it->second;
		m_Items.erase(it);
		return true;
	}
	return false;
}
KxAuiToolBarItem* KxAuiToolBar::GetByIterator(const KxAuiToolBarItem_ConstIterator& it) const
{
	if (it != m_Items.end())
	{
		return it->second;
	}
	return nullptr;
}

bool KxAuiToolBar::Create(wxWindow* parent,
						  wxWindowID id,
						  long style
)
{
	if (wxAuiToolBar::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		SetArtProvider(new KxAuiToolBarArt(this));

		Bind(wxEVT_MENU, &KxAuiToolBar::OnLeftClick, this);
		Bind(wxEVT_AUITOOLBAR_MIDDLE_CLICK, &KxAuiToolBar::EventHandler, this);
		Bind(wxEVT_AUITOOLBAR_RIGHT_CLICK, &KxAuiToolBar::EventHandler, this);
		Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &KxAuiToolBar::EventHandler, this);
		Bind(wxEVT_AUITOOLBAR_OVERFLOW_CLICK, &KxAuiToolBar::EventHandler, this);
		return true;
	}
	return false;
}
KxAuiToolBar::~KxAuiToolBar()
{
	Clear();
}

void KxAuiToolBar::ClearTools()
{
	wxAuiToolBar::ClearTools();
	Clear();
}

KxAuiToolBarItem* KxAuiToolBar::AddTool(const wxString& label, const wxBitmap& bitmap, wxItemKind kind, const wxString& shortHelp)
{
	return OnCreateTool(wxAuiToolBar::AddTool(wxID_ANY, label, bitmap, shortHelp, kind == wxITEM_DROPDOWN ? wxITEM_NORMAL : kind));
}
KxAuiToolBarItem* KxAuiToolBar::AddTool(const wxString& label, const wxBitmap& bitmap, const wxBitmap& disabledBitmap, wxItemKind kind, const wxString& shortHelp)
{
	return OnCreateTool(wxAuiToolBar::AddTool(wxID_ANY, label, bitmap, disabledBitmap, kind, shortHelp, wxEmptyString, nullptr));
}
KxAuiToolBarItem* KxAuiToolBar::AddTool(const wxBitmap& bitmap, const wxBitmap& disabledBitmap, bool toggle, const wxString& shortHelp, const wxString& longHelp)
{
	return OnCreateTool(wxAuiToolBar::AddTool(wxID_ANY, bitmap, disabledBitmap, toggle, nullptr, shortHelp, longHelp));;
}

KxAuiToolBarItem* KxAuiToolBar::AddLabel(const wxString& label, const int width)
{
	return OnCreateTool(wxAuiToolBar::AddLabel(wxID_ANY, label, width));
}
KxAuiToolBarItem* KxAuiToolBar::AddControl(wxControl* control, const wxString& label)
{
	return OnCreateTool(wxAuiToolBar::AddControl(control, label));
}
KxAuiToolBarItem* KxAuiToolBar::AddSeparator()
{
	return OnCreateTool(wxAuiToolBar::AddSeparator());
}
KxAuiToolBarItem* KxAuiToolBar::AddSpacer(int pixels)
{
	return OnCreateTool(wxAuiToolBar::AddSpacer(pixels));
}
KxAuiToolBarItem* KxAuiToolBar::AddStretchSpacer(int proportion)
{
	return OnCreateTool(wxAuiToolBar::AddStretchSpacer(proportion));
}

KxAuiToolBarItem* KxAuiToolBar::FindToolByPosition(const wxPoint& pos) const
{
	wxAuiToolBarItem* item = wxAuiToolBar::FindToolByPosition(pos.x, pos.y);
	if (item)
	{
		return GetByIterator(GetIteratorToTool(item->GetId()));
	}
	return nullptr;
}
KxAuiToolBarItem* KxAuiToolBar::FindToolByIndex(int index) const
{
	wxAuiToolBarItem* item = wxAuiToolBar::FindToolByIndex(index);
	if (item)
	{
		return GetByIterator(GetIteratorToTool(item->GetId()));
	}
	return nullptr;
}
KxAuiToolBarItem* KxAuiToolBar::FindToolByID(wxWindowID id) const
{
	return GetByIterator(GetIteratorToTool(id));
}

bool KxAuiToolBar::RemoveTool(KxAuiToolBarItem* tool)
{
	if (tool)
	{
		return RemoveByIterator(GetIteratorToTool(tool->GetID()));
	}
	return false;
}
bool KxAuiToolBar::RemoveTool(int position)
{
	return RemoveTool(FindToolByIndex(position));
}

//////////////////////////////////////////////////////////////////////////
void KxAuiToolBarArt::DrawPlainBackground(wxDC& dc, wxWindow* window, const wxRect& rect)
{
	wxColour colorBG = m_Instance->GetBackgroundColour();
	if (colorBG.IsOk())
	{
		dc.SetBackground(colorBG);
		dc.Clear();
	}
	else
	{
		wxAuiDefaultToolBarArt::DrawPlainBackground(dc, window, rect);
	}

	wxColour colorBorder = m_Instance->GetBorderColor();
	if (colorBorder.IsOk())
	{
		dc.SetPen(colorBorder);
		dc.DrawLine(rect.GetLeftBottom(), rect.GetRightBottom() + wxPoint(1, 0));
	}
}
