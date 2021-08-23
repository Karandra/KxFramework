#include "KxfPCH.h"
#include "ToolBar.h"
#include "../ToolBar.h"
#include "../ToolBarItem.h"
#include <wx/wupdlock.h>
#include <wx/dc.h>

namespace
{
	constexpr bool IsValidID(wxWindowID id) noexcept
	{
		return id != wxID_NONE && id != wxID_SEPARATOR && id != wxID_ANY;
	}
}

namespace kxf::WXUI
{
	void ToolBar::EventHandler(wxAuiToolBarEvent& event)
	{
		if (IsValidID(event.GetToolId()))
		{
			if (wxAuiToolBarItem* item = FindTool(event.GetToolId()))
			{
				/*
				EventID type = event.GetEventType();
				if (type == wxEVT_AUITOOLBAR_RIGHT_CLICK)
				{
					type = AuiToolBarEvent::EvtItemRightClick;
				}
				else if (type == wxEVT_AUITOOLBAR_MIDDLE_CLICK)
				{
					type = AuiToolBarEvent::EvtItemMiddleClick;
				}
				else if (type == wxEVT_AUITOOLBAR_TOOL_DROPDOWN)
				{
					type = AuiToolBarEvent::EvtItemDropdown;
				}
				else if (type == wxEVT_AUITOOLBAR_OVERFLOW_CLICK)
				{
					type = AuiToolBarEvent::EvtOverflowClick;
				}
				else
				{
					type = wxEVT_NULL;
				}

				if (type != wxEVT_NULL)
				{
					AuiToolBarEvent newEvent(event);
					newEvent.SetId(GetId());
					newEvent.SetEventType(type.AsInt());
					newEvent.SetEventObject(item);
					newEvent.SetInt(item->IsToggled());
					newEvent.SetItem(item);

					item->ProcessEvent(newEvent);
				}
				*/
			}
		}
		event.Skip();
	}
	void ToolBar::OnLeftClick(wxCommandEvent& event)
	{
		if (IsValidID(event.GetId()))
		{
			if (wxAuiToolBarItem* item = FindTool(event.GetId()))
			{
				/*
				AuiToolBarEvent newEvent(AuiToolBarEvent::EvtItemClick, GetId());
				newEvent.SetEventObject(item);
				newEvent.SetToolId(item->GetID());
				newEvent.SetInt(item->IsToggled());
				newEvent.SetItem(item);
				item->ProcessEvent(newEvent);
				*/
			}
		}
		event.Skip();
	}

	std::shared_ptr<IToolBarWidgetItem> ToolBar::DoCreateItem(wxAuiToolBarItem* item, size_t index, WidgetID id)
	{
		if (item)
		{
			DoSetItemIndex(*item, index);

			auto ptr = m_Items.insert_or_assign(item, std::make_shared<Widgets::ToolBarItem>(m_Widget, *item)).first->second;
			ptr->SetID(id);

			return ptr;
		}
		return nullptr;
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::DoGetItem(const wxAuiToolBarItem& item)
	{
		auto it = m_Items.find(&item);
		return it != m_Items.end() ? it->second : nullptr;
	}
	bool ToolBar::DoRemoveItem(wxAuiToolBarItem& item)
	{
		auto it = m_Items.find(&item);
		if (it != m_Items.end())
		{
			it->second->OnWXItemDestroyed();

			wxAuiToolBar::DeleteTool(item.GetId());
			m_Items.erase(it);

			return true;
		}
		return false;
	}

	size_t ToolBar::DoGetItemIndex(const wxAuiToolBarItem& item) const
	{
		const auto& items = wxAuiToolBar::m_items;
		for (size_t i = 0; i < items.size(); i++)
		{
			if (&items[i] == &item)
			{
				return i;
			}
		}
		return IToolBarWidget::npos;
	}
	bool ToolBar::DoSetItemIndex(wxAuiToolBarItem& item, size_t newIndex)
	{
		if (newIndex < GetToolCount())
		{
			auto& items = wxAuiToolBar::m_items;
			for (size_t i = 0; i < items.size(); i++)
			{
				if (&items[i] == &item)
				{
					if (i == newIndex)
					{
						return true;
					}

					items.Insert(item, newIndex, 1);
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

	ToolBar::ToolBar(Widgets::ToolBar& widget)
		:EvtHandlerWrapper(widget), m_Widget(widget)
	{
	}
	ToolBar::~ToolBar()
	{
		Clear();
	}

	bool ToolBar::Create(wxWindow* parent,
						 const String& label,
						 const Point& pos,
						 const Size& size
	)
	{
		if (wxAuiToolBar::Create(parent, wxID_NONE, pos, size, wxAUI_TB_HORIZONTAL|wxAUI_TB_TEXT|wxAUI_TB_OVERFLOW|wxAUI_TB_PLAIN_BACKGROUND))
		{
			SetLabel(label);
			EnableSystemTheme();
			SetMargins(0, 0, 0, 0);
			SetArtProvider(new Private::ToolBarRenderer(*this));

			Bind(wxEVT_MENU, &ToolBar::OnLeftClick, this);
			Bind(wxEVT_AUITOOLBAR_MIDDLE_CLICK, &ToolBar::EventHandler, this);
			Bind(wxEVT_AUITOOLBAR_RIGHT_CLICK, &ToolBar::EventHandler, this);
			Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &ToolBar::EventHandler, this);
			Bind(wxEVT_AUITOOLBAR_OVERFLOW_CLICK, &ToolBar::EventHandler, this);
			return true;
		}
		return false;
	}

	std::shared_ptr<IToolBarWidgetItem> ToolBar::AddItem(size_t index, const String& label, wxItemKind kind, WidgetID id)
	{
		return DoCreateItem(wxAuiToolBar::AddTool(wxID_ANY, label, wxNullBitmap, {}, kind), index, id);
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::AddWidget(size_t index, wxControl* control, WidgetID id)
	{
		control->Reparent(this);
		return DoCreateItem(wxAuiToolBar::AddControl(control), index, id);
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::AddSeparator(size_t index, WidgetID id)
	{
		return DoCreateItem(wxAuiToolBar::AddSeparator(), index, id);
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::AddSpacer(size_t index, int size, WidgetID id)
	{
		return DoCreateItem(wxAuiToolBar::AddSpacer(size), index, id);
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::AddStretchSpacer(size_t index, int proportion, WidgetID id)
	{
		return DoCreateItem(wxAuiToolBar::AddStretchSpacer(proportion), index, id);
	}
	bool ToolBar::RemoveItem(size_t index)
	{
		const auto& items = wxAuiToolBar::m_items;
		if (index < items.size())
		{
			return DoRemoveItem(items[index]);
		}
		return false;
	}
	void ToolBar::Clear()
	{
		for (auto& [itemWx, item]: m_Items)
		{
			item->OnWXItemDestroyed();
		}
		m_Items.clear();

		wxAuiToolBar::ClearTools();
	}

	std::shared_ptr<IToolBarWidgetItem> ToolBar::FindToolByPosition(const Point& pos)
	{
		if (wxAuiToolBarItem* item = wxAuiToolBar::FindToolByPosition(pos.GetX(), pos.GetY()))
		{
			return DoGetItem(*item);
		}
		return nullptr;
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::FindToolByIndex(size_t index)
	{
		if (index >= m_Items.size())
		{
			return nullptr;
		}

		if (wxAuiToolBarItem* item = wxAuiToolBar::FindToolByIndex(static_cast<int>(index)))
		{
			return DoGetItem(*item);
		}
		return nullptr;
	}
	std::shared_ptr<IToolBarWidgetItem> ToolBar::FindToolByID(const WidgetID& id)
	{
		for (auto& [itemWx, item]: m_Items)
		{
			if (item->GetID() == id)
			{
				return item;
			}
		}
		return nullptr;
	}

	void ToolBar::UpdateUI()
	{
		wxWindowUpdateLocker lock1(GetParent());
		wxWindowUpdateLocker lock2(this);

		Realize();
		GetParent()->Layout();
		ScheduleRefresh();
	}
}

namespace kxf::WXUI::Private
{
	void ToolBarRenderer::DrawPlainBackground(wxDC& dc, wxWindow* window, const wxRect& rect)
	{
		if (Color colorBG = m_ToolBar->GetBackgroundColour())
		{
			dc.SetBackground(colorBG);
			dc.Clear();
		}
		else
		{
			wxAuiDefaultToolBarArt::DrawPlainBackground(dc, window, rect);
		}

		if (Color colorBorder = m_ToolBar->GetBorderColor())
		{
			dc.SetPen(colorBorder);
			dc.DrawLine(rect.GetLeftBottom(), rect.GetRightBottom() + m_ToolBar->FromDIP(wxPoint(1, 0)));
		}
	}
}
