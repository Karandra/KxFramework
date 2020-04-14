#include "KxStdAfx.h"
#include "KxFramework/KxAuiToolBar.h"
#include "KxFramework/KxAuiToolBarItem.h"
#include "KxFramework/KxMenu.h"

namespace
{
	bool IsValidID(wxWindowID id)
	{
		return id != wxID_NONE && id != wxID_SEPARATOR && id != wxID_ANY;
	}
}

wxIMPLEMENT_DYNAMIC_CLASS(KxAuiToolBar, wxAuiToolBar);

void KxAuiToolBar::EventHandler(wxAuiToolBarEvent& event)
{
	if (IsValidID(event.GetToolId()))
	{
		if (KxAuiToolBarItem* item = FindToolByID(event.GetToolId()))
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

KxAuiToolBarItem* KxAuiToolBar::DoCreateTool(wxAuiToolBarItem* item)
{
	if (item)
	{
		auto it = m_Items.insert_or_assign(item->GetId(), std::make_unique<KxAuiToolBarItem>(*this, *item));
		return it.first->second.get();
	}
	return nullptr;
}
KxAuiToolBarItem* KxAuiToolBar::DoGetTool(const wxAuiToolBarItem& item)
{
	auto it = m_Items.find(item.GetId());
	return it != m_Items.end() ? it->second.get() : nullptr;
}
bool KxAuiToolBar::DoRemoveTool(wxAuiToolBarItem& item)
{
	auto it = m_Items.find(item.GetId());
	if (it != m_Items.end())
	{
		wxAuiToolBar::DeleteTool(item.GetId());
		m_Items.erase(it);

		return true;
	}
	return false;
}

size_t KxAuiToolBar::DoGetToolIndex(const KxAuiToolBarItem& item) const
{
	const auto& items = wxAuiToolBar::m_items;
	for (size_t i = 0; i < items.size(); i++)
	{
		if (&items[i] == item.m_Item)
		{
			return i;
		}
	}
	return KxAuiToolBarItem::npos;
}
bool KxAuiToolBar::DoSetToolIndex(KxAuiToolBarItem& item, size_t newIndex)
{
	if (newIndex < GetToolCount() && item.GetIndex() != newIndex)
	{
		auto& items = wxAuiToolBar::m_items;
		for (size_t i = 0; i < items.size(); i++)
		{
			if (&items[i] == item.m_Item)
			{
				items.Insert(*item.m_Item, newIndex, 1);
				if (newIndex > i)
				{
					items.RemoveAt(i);
				}
				else
				{
					items.RemoveAt(i + 1);
				}

				return true;
			}
		}
	}
	return false;
}

bool KxAuiToolBar::Create(wxWindow* parent,
						  wxWindowID id,
						  long style
)
{
	if (wxAuiToolBar::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		SetMargins(0, 0, 0, 0);
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
	m_Items.clear();
	wxAuiToolBar::ClearTools();
}

KxAuiToolBarItem* KxAuiToolBar::AddTool(const wxString& label, const wxBitmap& bitmap, wxItemKind kind, const wxString& shortHelp)
{
	return DoCreateTool(wxAuiToolBar::AddTool(wxID_ANY, label, bitmap, shortHelp, kind == wxITEM_DROPDOWN ? wxITEM_NORMAL : kind));
}
KxAuiToolBarItem* KxAuiToolBar::AddTool(const wxString& label, const wxBitmap& bitmap, const wxBitmap& disabledBitmap, wxItemKind kind, const wxString& shortHelp)
{
	return DoCreateTool(wxAuiToolBar::AddTool(wxID_ANY, label, bitmap, disabledBitmap, kind, shortHelp, wxEmptyString, nullptr));
}
KxAuiToolBarItem* KxAuiToolBar::AddTool(const wxBitmap& bitmap, const wxBitmap& disabledBitmap, bool toggle, const wxString& shortHelp, const wxString& longHelp)
{
	return DoCreateTool(wxAuiToolBar::AddTool(wxID_ANY, bitmap, disabledBitmap, toggle, nullptr, shortHelp, longHelp));;
}

KxAuiToolBarItem* KxAuiToolBar::AddLabel(const wxString& label, const int width)
{
	return DoCreateTool(wxAuiToolBar::AddLabel(wxID_ANY, label, width));
}
KxAuiToolBarItem* KxAuiToolBar::AddControl(wxControl* control, const wxString& label)
{
	return DoCreateTool(wxAuiToolBar::AddControl(control, label));
}
KxAuiToolBarItem* KxAuiToolBar::AddSeparator()
{
	return DoCreateTool(wxAuiToolBar::AddSeparator());
}
KxAuiToolBarItem* KxAuiToolBar::AddSpacer(int pixels)
{
	return DoCreateTool(wxAuiToolBar::AddSpacer(pixels));
}
KxAuiToolBarItem* KxAuiToolBar::AddStretchSpacer(int proportion)
{
	return DoCreateTool(wxAuiToolBar::AddStretchSpacer(proportion));
}

KxAuiToolBarItem* KxAuiToolBar::FindToolByPosition(const wxPoint& pos) const
{
	if (wxAuiToolBarItem* item = wxAuiToolBar::FindToolByPosition(pos.x, pos.y))
	{
		return const_cast<KxAuiToolBar&>(*this).DoGetTool(*item);
	}
	return nullptr;
}
KxAuiToolBarItem* KxAuiToolBar::FindToolByIndex(int index) const
{
	if (wxAuiToolBarItem* item = wxAuiToolBar::FindToolByIndex(index))
	{
		return const_cast<KxAuiToolBar&>(*this).DoGetTool(*item);
	}
	return nullptr;
}
KxAuiToolBarItem* KxAuiToolBar::FindToolByID(wxWindowID id) const
{
	if (wxAuiToolBarItem* item = wxAuiToolBar::FindTool(id))
	{
		return const_cast<KxAuiToolBar&>(*this).DoGetTool(*item);
	}
	return nullptr;
}

bool KxAuiToolBar::RemoveTool(KxAuiToolBarItem& tool)
{
	return DoRemoveTool(tool);
}
bool KxAuiToolBar::RemoveTool(int index)
{
	if (KxAuiToolBarItem* item = FindToolByIndex(index))
	{
		return RemoveTool(*item);
	}
	return false;
}

void KxAuiToolBar::UpdateUI()
{
	wxWindowUpdateLocker lock1(GetParent());
	wxWindowUpdateLocker lock2(this);

	Realize();
	GetParent()->Layout();
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
