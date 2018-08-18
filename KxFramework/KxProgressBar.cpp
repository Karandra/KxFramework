#include "KxStdAfx.h"
#include "KxFramework/KxProgressBar.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxProgressBar, wxGauge);

void KxProgressBar::OnPaint(wxPaintEvent& hEvent)
{
	if (!m_InPulseMode)
	{
		wxPaintDC dc(this);
		wxGauge::OnPaint(hEvent);
		wxRect rect(wxPoint(0, 0), GetSize());

		// Draw label
		if (HasFlag(wxGA_TEXT) && !GetLabel().IsEmpty())
		{
			float value = ((float)GetValue()/(float)GetRange())*100.0f;
			wxString label = wxString::Format(GetLabel(), value);
			dc.DrawLabel(label, rect, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
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

void KxProgressBar::Pulse()
{
	if (!m_InPulseMode)
	{
		wxGauge::Pulse();
	}
	m_InPulseMode = true;
}
void KxProgressBar::SetValue(int pos)
{
	m_InPulseMode = false;
	wxGauge::SetValue(pos);
}
void KxProgressBar::SetValue(int64_t current, int64_t max)
{
	int range = GetRange();

	int value = 0;
	if (max != 0)
	{
		value = ((double)current / (double)max) * range;
	}
	else if (max > current)
	{
		value = range;
	}
	SetValue(value);
}

void KxProgressBar::Advance(int value)
{
	int max = GetRange();
	value += GetValue();

	SetValue(value <= max ? value : max);
}
