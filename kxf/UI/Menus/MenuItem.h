#pragma once
#include "kxf/UI/Common.h"
#include "MenuEvent.h"
#include <wx/menuitem.h>

namespace kxf::UI
{
	class Menu;
}

namespace kxf::UI
{
	class KX_API MenuItem: public wxEvtHandler, public wxMenuItem
	{
		friend class Menu;

		protected:
			template<class T>
			constexpr static bool TestIDType() noexcept
			{
				return std::is_integral_v<T> || std::is_enum_v<T>;
			}

		private:
			wxWindowID m_EffectiveID = wxID_NONE;

		private:
			void OnCreate();
			bool OnMeasureItem(size_t* width, size_t* height) override;
			bool OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status) override;

			void OnAddedToMenu();
			void OnRemovedFromMenu();

			void CheckIfShouldOwnerDraw();
			wxWindowID GetEffectiveID(wxWindowID id) const;

		public:
			template<class T, class = std::enable_if_t<TestIDType<T>()>>
			MenuItem(T id, const String& label = {}, const String& helpString = {}, wxItemKind kind = wxITEM_NORMAL)
				:MenuItem(static_cast<wxWindowID>(id), label, helpString, kind)
			{
			}

			MenuItem(wxWindowID id, const String& label = {}, const String& helpString = {}, wxItemKind kind = wxITEM_NORMAL);
			MenuItem(const String& label, const String& helpString = {}, wxItemKind kind = wxITEM_NORMAL);

		public:
			wxEvtHandler* GetEventHandler()
			{
				return this;
			}
			const wxEvtHandler* GetEventHandler() const
			{
				return this;
			}
			
			bool HasEffectiveID() const
			{
				return m_EffectiveID >= 0;
			}
			wxWindowID GetId() const
			{
				return HasEffectiveID() ? m_EffectiveID : wxMenuItem::GetId();
			}
			
			wxWindow* GetWindow() const;
			wxMenu* GetMenuWx() const
			{
				return wxMenuItem::GetMenu();
			}
			wxMenu* GetSubMenuWx() const
			{
				return wxMenuItem::GetSubMenu();
			}
			Menu* GetMenu() const;
			Menu* GetSubMenu() const;

			void SetMenu(Menu* menu);
			void SetSubMenu(Menu* subMenu);

			virtual void SetBitmap(const GDIBitmap& normal, const GDIBitmap& checked = {});
			virtual bool SetDefault();

		public:
			wxDECLARE_DYNAMIC_CLASS(MenuItem);
	};
}
