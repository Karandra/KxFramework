#pragma once
#include "../Common.h"
#include <wx/menu.h>
#include <wx/menuitem.h>

namespace kxf::Widgets
{
	class MenuWidgetItem;
}

namespace kxf::WXUI
{
	class KX_API MenuItem: public wxMenuItem
	{
		private:
			Widgets::MenuWidgetItem& m_Item;

		protected:
			bool OnMeasureItem(size_t* width, size_t* height) override;
			bool OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status) override;

		public:
			MenuItem(Widgets::MenuWidgetItem& item, wxMenu& subMenu) noexcept;
			MenuItem(Widgets::MenuWidgetItem& item, wxItemKind itemKind) noexcept;
			~MenuItem();

		public:
			wxWindow* GetWindow() const;
	};
}
