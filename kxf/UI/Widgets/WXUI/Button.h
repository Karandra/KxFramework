#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include "kxf/UI/IButtonWidget.h"
#include "kxf/Drawing/IRendererNative.h"
#include <wx/anybutton.h>
#include <wx/systhemectrl.h>

namespace kxf::Widgets
{
	class Button;
}

namespace kxf::WXUI
{
	class KX_API Button: public UI::WindowRefreshScheduler<wxSystemThemedControl<wxAnyButton>>
	{
		public:
			static Size GetDefaultSize();

		private:
			Widgets::Button& m_Widget;
			wxEvtHandler m_EventHandler;

			NativeWidgetFlag m_WidgetState = NativeWidgetFlag::None;
			bool m_IsDropdownEnbled = false;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnResize(wxSizeEvent& event);
			void OnKillFocus(wxFocusEvent& event);
			void OnMouseEnter(wxMouseEvent& event);
			void OnMouseLeave(wxMouseEvent& event);
			void OnLeftButtonUp(wxMouseEvent& event);
			void OnLeftButtonDown(wxMouseEvent& event);

		protected:
			wxSize DoGetBestSize() const override;
			wxSize DoGetBestClientSize() const override;
			wxSize DoGetSizeFromTextSize(int xlen, int ylen = -1) const override;

		public:
			Button(Widgets::Button& widget)
				:m_Widget(widget)
			{
			}
			~Button()
			{
				if (m_EventHandler.GetClientData() == this)
				{
					PopEventHandler();
				}
			}

		public:
			bool Create(wxWindow* parent,
						const String& label,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize()
			);

		public:
			bool Enable(bool enable = true) override
			{
				ScheduleRefresh();
				return wxAnyButton::Enable(enable);
			}
			void SetLabel(const wxString& label) override
			{
				ScheduleRefresh();
				wxAnyButton::SetLabel(label);
			}
		
			bool IsDefaultButton() const;
			wxWindow* SetDefaultButton();

			bool IsDropdownEnabled() const
			{
				return m_IsDropdownEnbled;
			}
			void SetDropdownEnbled(bool show = true)
			{
				m_IsDropdownEnbled = show;
				ScheduleRefresh();
			}
	};
}
