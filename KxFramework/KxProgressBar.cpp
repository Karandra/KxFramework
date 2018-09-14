#include "KxStdAfx.h"
#include "KxFramework/KxProgressBar.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxProgressBar, wxGauge);

void KxProgressBar::OnPaint(wxPaintEvent& event)
{
	if (!m_InPulseMode)
	{
		wxPaintDC dc(this);
		wxGauge::OnPaint(event);
		wxRect rect(wxPoint(0, 0), GetSize());

		// Draw label
		const wxString label = GetLabel();
		if (HasFlag(wxGA_TEXT) && !label.IsEmpty())
		{
			double value = ((double)DoGetValue() / (double)DoGetRange()) * 100.0;
			dc.DrawLabel(wxString::Format(label, value), rect, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
		}
	}
}

bool KxProgressBar::Create(wxWindow* parent,
						   wxWindowID id,
						   int range,
						   long style
)
{
	if (wxGauge::Create(parent, id, range, wxDefaultPosition, wxDefaultSize, style))
	{
		Bind(wxEVT_PAINT, &KxProgressBar::OnPaint, this);
		return true;
	}
	return false;
}
