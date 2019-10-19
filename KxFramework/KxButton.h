#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithDropdownMenu.h"
#include "KxEvent.h"

KxEVENT_DECLARE_GLOBAL(BUTTON, wxCommandEvent);
KxEVENT_DECLARE_GLOBAL(BUTTON_MENU, wxContextMenuEvent);

class KX_API KxButton: public wxSystemThemedControl<wxAnyButton>, public KxWithDropdownMenu
{
	public:
		static wxSize GetDefaultSize();

	private:
		wxEvtHandler m_EventHandler;

		int m_ControlState = wxCONTROL_NONE;
		bool m_IsSliptterEnabled = false;
		bool m_IsFocusDrawingAllowed = false;
		bool m_IsAuthNeeded = false;
		bool m_ShouldRefresh = false;

	private:
		void OnPaint(wxPaintEvent& event);
		void OnResize(wxSizeEvent& event);
		void OnMouseLeave(wxMouseEvent& event);
		void OnMouseEnter(wxMouseEvent& event);
		void OnLeftButtonUp(wxMouseEvent& event);
		void OnLeftButtonDown(wxMouseEvent& event);

	protected:
		wxSize DoGetBestSize() const override;
		wxSize DoGetBestClientSize() const override;
		void OnInternalIdle() override;

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
		virtual wxWindow* SetDefault();

		wxSize GetBitmapMargins() const
		{
			return const_cast<KxButton*>(this)->wxAnyButton::GetBitmapMargins();
		}
		void SetBitmapMargins(wxCoord x, wxCoord y)
		{
			wxAnyButton::SetBitmapMargins(x, y);
		}
		void SetBitmapMargins(const wxSize& margins)
		{
			wxAnyButton::SetBitmapMargins(margins);
		}

		bool IsSplitterEnabled()
		{
			return m_IsSliptterEnabled;
		}
		void SetSplitterEnabled(bool show = true)
		{
			m_ShouldRefresh = true;
			m_IsSliptterEnabled = show;
		}
		
		bool IsAuthNeeded()
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
			m_IsFocusDrawingAllowed = value;
			m_ShouldRefresh = true;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxButton);
};
