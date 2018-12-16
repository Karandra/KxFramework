#pragma once
#include "KxFramework/KxFramework.h"

enum
{
	KxLABEL_NONE = 0,

	KxLABEL_CAPTION = 1 << 0,
	KxLABEL_LINE = 1 << 1,
	KxLABEL_SELECTION = 1 << 2,
	KxLABEL_HYPERLINK = 1 << 3,
	KxLABEL_COLORED = 1 << 4,
};

class KX_API KxLabel: public wxSystemThemedControl<wxStaticText>
{
	private:
		static const wxEllipsizeMode LabelEllipsizeMode = wxELLIPSIZE_END;
		static const int MinSingleLineHeight = 23;

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
		long m_Style = DefaultStyle;
		bool m_IsMultilne = false;
		int m_MultiLineAlignStyle = wxALIGN_LEFT|wxALIGN_TOP;
		int m_AlignStyle = wxALIGN_CENTER_VERTICAL;
		
	private:
		void OnPaint(wxPaintEvent& event);
		void OnEnter(wxMouseEvent& event);
		void OnLeave(wxMouseEvent& event);
		void OnMouseDown(wxMouseEvent& event);
		void OnMouseUp(wxMouseEvent& event);

		virtual bool AcceptsFocus() const
		{
			return false;
		}
		virtual bool AcceptsFocusFromKeyboard() const
		{
			return false;
		}

		const wxColour& GetStateColor() const;
		void SetupColors(const wxColour& color)
		{
			wxWindowUpdateLocker tLock(this);

			m_ColorNormal = color;
			m_ColorHighlight = color;
			m_ColorClick = color;
			m_ColorDisabled = wxColour(color).MakeDisabled();
			Refresh();
		}
		bool IsLabelMultiline(const wxString& label)
		{
			return label.Find('\r') != -1 || label.Find('\n') != -1;
		}
		wxSize CalcBestSize(wxDC* dc = NULL);

	protected:
		virtual void DoEnable(bool enable) override
		{
			Refresh();
			return wxStaticText::DoEnable(enable);
		}
		virtual wxSize DoGetBestSize() const override
		{
			return m_BestSize;
		}
		virtual wxSize GetMinSize() const override
		{
			return DoGetBestSize();
		}
		void DoSetLabel(const wxString& label)
		{
			m_Label = label;
			m_IsMultilne = IsLabelMultiline(label);
			m_BestSize = CalcBestSize();
			Refresh();
		}

	public:
		static const long DefaultStyle = KxLABEL_NONE;
		
		KxLabel() {}
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
		virtual long GetWindowStyleFlag() const override
		{
			return m_Style;
		}
		virtual void SetWindowStyleFlag(long style) override
		{
			m_Style = style;
		}
		virtual wxString GetLabel() const override
		{
			return m_Label;
		}
		virtual void SetLabel(const wxString& label) override
		{
			DoSetLabel(label);
		}
		virtual void SetLabelText(const wxString& label) override
		{
			DoSetLabel(label);
		}
		virtual bool SetForegroundColour(const wxColour& color) override
		{
			SetupColors(color);
			return wxStaticText::SetForegroundColour(color);
		}

		wxBitmap GetBitmap()
		{
			return m_Icon;
		}
		void SetBitmap(const wxBitmap& image);
		void Wrap(int width)
		{
			m_WrapLength = width;
		}
		wxColour GetNormalColor()
		{
			return m_ColorNormal;
		}
		void SetNormalColor(const wxColour& color)
		{
			m_ColorNormal = color;
			Refresh();
		}
		wxColour GetHighlightColor()
		{
			return m_ColorHighlight;
		}
		void SetHighlightColor(const wxColour& color)
		{
			m_ColorHighlight = color;
			Refresh();
		}
		wxColour GetClickColor()
		{
			return m_ColorClick;
		}
		void SetClickColor(const wxColour& color)
		{
			m_ColorClick = color;
			Refresh();
		}

		void SetLabelAlignment(int singleLine, int multiLine = wxALIGN_LEFT|wxALIGN_TOP)
		{
			m_AlignStyle = singleLine;
			m_MultiLineAlignStyle = multiLine;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxLabel);
};
