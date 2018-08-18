#include "KxStdAfx.h"
#include "KxFramework/KxStaticText.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxStaticText, wxStaticText)

void KxStaticText::OnEnter(wxMouseEvent& event)
{
	SetForegroundColour(m_ColorHighlight);
	Refresh();
	event.Skip();
}
void KxStaticText::OnLeave(wxMouseEvent& event)
{
	SetForegroundColour(m_ColorNormal);
	Refresh();
	event.Skip();
}
void KxStaticText::OnMouseDown(wxMouseEvent& event)
{
	SetForegroundColour(m_ColorClick);
	Refresh();
	event.Skip();
}

bool KxStaticText::Create(wxWindow* parent,
						  wxWindowID id,
						  const wxString& label,
						  long style
)
{
	if (wxStaticText::Create(parent, id, label, wxDefaultPosition, wxDefaultSize, style))
	{
		m_ColorNormal = GetForegroundColour();
		m_ColorHighlight = m_ColorNormal;
		m_ColorClick = m_ColorNormal;

		Bind(wxEVT_ENTER_WINDOW, &KxStaticText::OnEnter, this);
		Bind(wxEVT_LEAVE_WINDOW, &KxStaticText::OnLeave, this);
		Bind(wxEVT_LEFT_UP, &KxStaticText::OnLeave, this);
		Bind(wxEVT_LEFT_DOWN, &KxStaticText::OnMouseDown, this);
		return true;
	}
	return false;
}
KxStaticText::~KxStaticText()
{
}
