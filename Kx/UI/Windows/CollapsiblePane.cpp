#include "stdafx.h"
#include "CollapsiblePane.h"
#include "Panel.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(CollapsiblePane, wxCollapsiblePane)

	void CollapsiblePane::OnCollapsedExpanded(wxCollapsiblePaneEvent& event)
	{
		GetParent()->Layout();
		if (dynamic_cast<ScrolledPanel*>(GetParent()))
		{
			GetParent()->FitInside();
		}
	}

	bool CollapsiblePane::Create(wxWindow* parent,
								 wxWindowID id,
								 const wxString& label,
								 CollapsiblePaneStyle style
	)
	{
		if (wxCollapsiblePane::Create(parent, id, label, wxDefaultPosition, wxDefaultSize, ToInt(style), wxDefaultValidator))
		{
			SetDoubleBuffered(true);
			if (ShouldInheritColours())
			{
				SetBackgroundColour(parent->GetBackgroundColour());
				SetForegroundColour(parent->GetForegroundColour());

				if (wxWindow* control = GetControlWidget())
				{
					control->SetBackgroundColour(parent->GetBackgroundColour());
					control->SetForegroundColour(parent->GetForegroundColour());
				}
			}

			m_EvtHandler.Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &CollapsiblePane::OnCollapsedExpanded, this);
			return true;
		}
		return false;
	}
}