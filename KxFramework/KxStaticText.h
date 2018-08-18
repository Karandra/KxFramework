#pragma once
#include "KxFramework/KxFramework.h"

class KxStaticText: public wxStaticText
{
	private:
		wxColour m_ColorNormal;
		wxColour m_ColorHighlight;
		wxColour m_ColorClick;

	private:
		void OnEnter(wxMouseEvent& event);
		void OnLeave(wxMouseEvent& event);
		void OnMouseDown(wxMouseEvent& event);

	public:
		static const long DefaultStyle = 0;

		KxStaticText() {}
		KxStaticText(wxWindow* parent,
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
		virtual ~KxStaticText();

	public:
		wxColour GetNormalColor()
		{
			return m_ColorNormal;
		}
		void SetNormalColor(const wxColour& color)
		{
			m_ColorNormal = color;
			SetForegroundColour(m_ColorNormal);
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

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStaticText);
};
