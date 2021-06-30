#include "KxfPCH.h"
#include "MenuItem.h"
#include "../MenuWidget.h"
#include "../MenuWidgetItem.h"

namespace kxf::WXUI
{
	bool MenuItem::OnMeasureItem(size_t* width, size_t* height)
	{
		const bool result = wxMenuItem::OnMeasureItem(width, height);
		return result;

		if (const wxWindow* window = GetWindow())
		{
			if (!IsSeparator())
			{
				if (height)
				{
					const wxBitmap& bitmap = GetBitmap(IsChecked());
					const int margin = window->FromDIP(Size(wxDefaultCoord, 6)).GetHeight();

					*height = std::max({(int)*height, wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y) + margin, bitmap.GetHeight() + margin});
				}
			}
		}
		return result;
	}
	bool MenuItem::OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status)
	{
		return wxMenuItem::OnDrawItem(dc, rect, action, status);
	}

	MenuItem::MenuItem(std::shared_ptr<Widgets::MenuWidgetItem> item, std::shared_ptr<IMenuWidget> subMenuRef, wxMenu& subMenu) noexcept
		:wxMenuItem(nullptr, wxID_ANY, {}, {}, wxITEM_NORMAL, &subMenu), m_SubMenu(std::move(subMenuRef)), m_Item(std::move(item))
	{
	}
	MenuItem::MenuItem(std::shared_ptr<Widgets::MenuWidgetItem> item, wxItemKind itemKind) noexcept
		:wxMenuItem(nullptr, (itemKind == wxITEM_SEPARATOR ? wxID_SEPARATOR : wxID_ANY), {}, {}, itemKind), m_Item(std::move(item))
	{
	}
	MenuItem::~MenuItem()
	{
		m_Item->OnWXMenuDestroyed();
	}

	wxWindow* MenuItem::GetWindow() const
	{
		if (wxMenu* menu = GetMenu())
		{
			return menu->GetWindow();
		}
		return nullptr;
	}
}
