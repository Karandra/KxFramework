#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithDropdownMenu.h"
#include "KxEvent.h"

KxEVENT_DECLARE_GLOBAL(BUTTON, wxCommandEvent);
KxEVENT_DECLARE_GLOBAL(BUTTON_MENU, wxContextMenuEvent);

class KX_API KxButton: public wxSystemThemedControl<wxButton>, public KxWithDropdownMenu
{
	public:
		enum
		{
			DRAW_STYLE_NORMAL,
			DRAW_STYLE_AERO,
			DRAW_STYLE_MAX
		};

		static wxSize GetDefaultSize();

	private:
		static const int ms_ArrowButtonWidth = 17;
		static const int ms_DefaultButtonHeight = 23;

	private:
		int m_ControlState = wxCONTROL_NONE;
		bool m_IsSliptterEnabled = false;
		bool m_IsFocusDrawingAllowed = false;
		
		bool m_IsAuthNeeded = false;

	private:
		void OnPaint(wxPaintEvent& event);
		void OnMouseLeave(wxMouseEvent& event);
		void OnMouseEnter(wxMouseEvent& event);
		void OnLeftButtonUp(wxMouseEvent& event);
		void OnLeftButtonDown(wxMouseEvent& event);

	protected:
		wxSize DoGetBestSize() const override;
		wxSize DoGetBestClientSize() const override;

	public:
		KxButton() {}
		KxButton(wxWindow* parent,
				  wxWindowID id,
				  const wxString& label,
				  const wxPoint& pos,
				  const wxSize& size,
				  long style = 0,
				  const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, label, pos, size, style, validator);
		}
		KxButton(wxWindow* parent,
				 wxWindowID id,
				 const wxString& label = wxEmptyString,
				 long style = 0,
				 const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, label, style, validator);
		}
		~KxButton();

	public:
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& label,
					const wxPoint& pos,
					const wxSize& size,
					long style = 0,
					const wxValidator& validator = wxDefaultValidator
		);
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& label = wxEmptyString,
					long style = 0,
					const wxValidator& validator = wxDefaultValidator
		)
		{
			return Create(parent, id, label, wxDefaultPosition, wxDefaultSize, style, validator);
		}

	public:
		bool Enable(bool enable = true) override;
		wxSize GetBitmapMargins() const
		{
			return const_cast<KxButton*>(this)->wxButton::GetBitmapMargins();
		}

		bool IsSplitterEnabled()
		{
			return m_IsSliptterEnabled;
		}
		void SetSplitterEnabled(bool show = true)
		{
			m_IsSliptterEnabled = show;
			Refresh();
		}
		
		bool IsAuthNeeded()
		{
			return m_IsAuthNeeded;
		}
		void SetAuthNeeded(bool show = true);
		
		void SetAllowDrawFocus(bool value)
		{
			m_IsFocusDrawingAllowed = value;
			Refresh();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxButton);
};
