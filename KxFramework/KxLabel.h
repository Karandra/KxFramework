#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWindowRefreshScheduler.h"

enum
{
	KxLABEL_NONE = 0,

	KxLABEL_CAPTION = 1 << 0,
	KxLABEL_LINE = 1 << 1,
	KxLABEL_SELECTION = 1 << 2,
	KxLABEL_HYPERLINK = 1 << 3,
	KxLABEL_COLORED = 1 << 4,
};

class KX_API KxLabel: public KxWindowRefreshScheduler<wxSystemThemedControl<wxStaticText>>
{
	private:
		wxEvtHandler m_EvtHandler;

		wxString m_Label;
		wxBitmap m_Icon = wxNullBitmap;
		wxBitmap m_IconDisabled = wxNullBitmap;
		wxColour m_ColorNormal;
		wxColour m_ColorHighlight;
		wxColour m_ColorClick;
		wxColour m_ColorDisabled;
		wxSize m_BestSize;

		int m_WrapLength = -1;
		int m_State = wxCONTROL_NONE;
		int m_Style = DefaultStyle;
		int m_MultiLineAlignStyle = wxALIGN_LEFT|wxALIGN_TOP;
		int m_AlignStyle = wxALIGN_CENTER_VERTICAL;
		bool m_IsMultilne = false;
		
	private:
		void OnPaint(wxPaintEvent& event);
		void OnEnter(wxMouseEvent& event);
		void OnLeave(wxMouseEvent& event);
		void OnMouseDown(wxMouseEvent& event);
		void OnMouseUp(wxMouseEvent& event);

		const wxColour& GetStateColor() const;
		void SetupColors(const wxColour& color)
		{
			ScheduleRefresh();

			m_ColorNormal = color;
			m_ColorHighlight = color;
			m_ColorClick = color;
			m_ColorDisabled = wxColour(color).MakeDisabled();
		}
		bool IsLabelMultiline(const wxString& label)
		{
			return label.Find('\r') != wxNOT_FOUND || label.Find('\n') != wxNOT_FOUND;
		}
		wxSize CalcBestSize(wxDC* dc = nullptr);

	protected:
		void DoEnable(bool enable) override
		{
			Refresh();
			return wxStaticText::DoEnable(enable);
		}
		wxSize DoGetBestSize() const override
		{
			return m_BestSize;
		}
		wxSize GetMinSize() const override
		{
			return DoGetBestSize();
		}
		void DoSetLabel(const wxString& label)
		{
			ScheduleRefresh();

			m_Label = label;
			m_IsMultilne = IsLabelMultiline(label);
			m_BestSize = CalcBestSize();
		}

	public:
		static const long DefaultStyle = KxLABEL_NONE;
		
		KxLabel() = default;
		KxLabel(wxWindow* parent,
				wxWindowID id,
				const wxString& label,
				long style = DefaultStyle
		)
		{
			Create(parent, id, label, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& label,
					long style = DefaultStyle
		);
		virtual ~KxLabel();

	public:
		long GetWindowStyleFlag() const override
		{
			return m_Style;
		}
		void SetWindowStyleFlag(long style) override
		{
			m_Style = style;
		}
		wxString GetLabel() const override
		{
			return m_Label;
		}
		void SetLabel(const wxString& label) override
		{
			DoSetLabel(label);
		}
		void SetLabelText(const wxString& label) override
		{
			DoSetLabel(label);
		}
		bool SetForegroundColour(const wxColour& color) override
		{
			SetupColors(color);
			return wxStaticText::SetForegroundColour(color);
		}

		bool AcceptsFocus() const override
		{
			return false;
		}
		bool AcceptsFocusFromKeyboard() const override
		{
			return false;
		}

		wxBitmap GetBitmap()
		{
			return m_Icon;
		}
		void SetBitmap(const wxBitmap& image);
		void Wrap(int width)
		{
			ScheduleRefresh();
			m_WrapLength = width;
		}
		wxColour GetNormalColor()
		{
			return m_ColorNormal;
		}
		void SetNormalColor(const wxColour& color)
		{
			ScheduleRefresh();
			m_ColorNormal = color;
		}
		wxColour GetHighlightColor()
		{
			return m_ColorHighlight;
		}
		void SetHighlightColor(const wxColour& color)
		{
			ScheduleRefresh();
			m_ColorHighlight = color;
		}
		wxColour GetClickColor()
		{
			return m_ColorClick;
		}
		void SetClickColor(const wxColour& color)
		{
			ScheduleRefresh();
			m_ColorClick = color;
		}

		void SetLabelAlignment(int singleLine, int multiLine = wxALIGN_LEFT|wxALIGN_TOP)
		{
			ScheduleRefresh();

			m_AlignStyle = singleLine;
			m_MultiLineAlignStyle = multiLine;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxLabel);
};
