#pragma once
#include "Kx/UI/Common.h"
#include "Kx/UI/WindowRefreshScheduler.h"
#include "Kx/UI/WithDropdownMenu.h"
#include <wx/renderer.h>
#include <wx/anybutton.h>
#include <wx/systhemectrl.h>

namespace KxFramework::UI
{
	enum class ButtonStyle
	{
		None = 0,

		AlignLeft = wxBU_LEFT,
		AlignTop = wxBU_TOP,
		AlignRight = wxBU_RIGHT,
		AlignBottom = wxBU_BOTTOM,
		ExactFit = wxBU_EXACTFIT,
		NoText = wxBU_NOTEXT
	};
}
namespace KxFramework
{
	Kx_DeclareFlagSet(UI::ButtonStyle);
}

namespace KxFramework::UI
{
	class KX_API Button: public WindowRefreshScheduler<wxSystemThemedControl<wxAnyButton>>, public WithDropdownMenu
	{
		public:
			static constexpr FlagSet<ButtonStyle> DefaultStyle = ButtonStyle::None;

			KxEVENT_MEMBER(wxCommandEvent, Click);
			KxEVENT_MEMBER(wxContextMenuEvent, Menu);

		public:
			static Size GetDefaultSize();

		private:
			wxEvtHandler m_EventHandler;

			int m_ControlState = wxCONTROL_NONE;
			bool m_IsSliptterEnabled = false;
			bool m_IsFocusDrawingAllowed = false;
			bool m_IsAuthNeeded = false;

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
			Button() = default;
			Button(wxWindow* parent,
				   wxWindowID id,
				   const String& label,
				   const Point& pos,
				   const Size& size,
				   FlagSet<ButtonStyle> style = DefaultStyle,
				   const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, label, pos, size, style, validator);
			}
			Button(wxWindow* parent,
				   wxWindowID id,
				   const String& label = {},
				   FlagSet<ButtonStyle> style = DefaultStyle,
				   const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, label, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& label,
						const Point& pos,
						const Size& size,
						FlagSet<ButtonStyle> style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& label = {},
						FlagSet<ButtonStyle> style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			)
			{
				return Create(parent, id, label, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), style, validator);
			}
			~Button()
			{
				if (m_EventHandler.GetClientData() == this)
				{
					PopEventHandler();
				}
			}

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
		
			bool IsDefault() const;
			virtual wxWindow* SetDefault();

			Size GetBitmapMargins() const
			{
				return const_cast<Button*>(this)->wxAnyButton::GetBitmapMargins();
			}
			void SetBitmapMargins(wxCoord x, wxCoord y)
			{
				wxAnyButton::SetBitmapMargins(x, y);
			}
			void SetBitmapMargins(const Size& margins)
			{
				wxAnyButton::SetBitmapMargins(margins);
			}

			bool IsSplitterEnabled() const
			{
				return m_IsSliptterEnabled;
			}
			void SetSplitterEnabled(bool show = true)
			{
				m_IsSliptterEnabled = show;
				ScheduleRefresh();
			}
		
			bool IsAuthNeeded() const
			{
				return m_IsAuthNeeded;
			}
			void SetAuthNeeded(bool show = true);
		
			bool IsFocusDrawingAllowed() const
			{
				return m_IsFocusDrawingAllowed;
			}
			void SetAllowDrawFocus(bool value)
			{
				ScheduleRefresh();
				m_IsFocusDrawingAllowed = value;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(Button);
	};
}
