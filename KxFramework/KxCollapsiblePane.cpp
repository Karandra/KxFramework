#include "KxStdAfx.h"
#include "KxFramework/KxCollapsiblePane.h"
#include "KxFramework/KxPanel.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxCollapsiblePane, wxCollapsiblePane)

void KxCollapsiblePane::OnCollapsedExpanded(wxCollapsiblePaneEvent& event)
{
	GetParent()->Layout();
	if (dynamic_cast<KxScrolledPanel*>(GetParent()))
	{
		GetParent()->FitInside();
	}
}

bool KxCollapsiblePane::Create(wxWindow* parent,
							   wxWindowID id,
							   const wxString& label,
							   long style
)
{
	if (wxCollapsiblePane::Create(parent, id, label, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator))
	{
		SetDoubleBuffered(true);
		if (ShouldInheritColours())
		{
			SetBackgroundColour(parent->GetBackgroundColour());
			SetForegroundColour(parent->GetForegroundColour());

			wxWindow* control = GetControlWidget();
			if (control)
			{
				control->SetBackgroundColour(parent->GetBackgroundColour());
				control->SetForegroundColour(parent->GetForegroundColour());
			}
		}

		m_EvtHandler.Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &KxCollapsiblePane::OnCollapsedExpanded, this);
		PushEventHandler(&m_EvtHandler);
		return true;
	}
	return false;
}
KxCollapsiblePane::~KxCollapsiblePane()
{
}
