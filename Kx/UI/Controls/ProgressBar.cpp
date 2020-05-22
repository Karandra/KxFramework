#include "stdafx.h"
#include "ProgressBar.h"
#include "Kx/General/StringFormater.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ProgressBar, wxGauge);

	void ProgressBar::OnPaint(wxPaintEvent& event)
	{
		if (!m_InPulseMode && HasFlag(wxGA_TEXT))
		{
			wxPaintDC dc(this);
			wxGauge::OnPaint(event);
			Rect rect(Point(0, 0), GetSize());

			// Draw label
			const String label = GetLabel();
			if (!label.IsEmpty())
			{
				StringFormatter::Formatter format(std::move(label));
				format(((double)DoGetValue() / (double)DoGetRange()) * 100.0, 2);

				dc.DrawLabel(format.ToString(), rect, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
			}
		}
	}

	bool ProgressBar::Create(wxWindow* parent,
							 wxWindowID id,
							 int range,
							 ProgressBarStyle style
	)
	{
		if (wxGauge::Create(parent, id, range, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), ToInt(style)))
		{
			Bind(wxEVT_PAINT, &ProgressBar::OnPaint, this);
			return true;
		}
		return false;
	}
}
