#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/UI/WindowRefreshScheduler.h"
#include "Kx/UI/WithDropdownMenu.h"
#include "KxEvent.h"

KxEVENT_DECLARE_GLOBAL(wxCommandEvent, BUTTON);
KxEVENT_DECLARE_GLOBAL(wxContextMenuEvent, BUTTON_MENU);

class KX_API KxButton:
	public KxFramework::WindowRefreshScheduler<wxSystemThemedControl<wxAnyButton>>,
	public KxFramework::WithDropdownMenu
{
	public:
		static wxSize GetDefaultSize();

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
		KxButton() = default;
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
				 const wxString& label = {},
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
					const wxString& label = {},
					long style = 0,
					const wxValidator& validator = wxDefaultValidator
		)
		{
			return Create(parent, id, label, wxDefaultPosition, wxDefaultSize, style, validator);
		}

	public:
		bool Enable(bool enable = true) override;
		void SetLabel(const wxString& label) override;
		
		bool IsDefault() const;
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
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxButton);
};
