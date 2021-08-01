#include "KxfPCH.h"
#include "ProgressMeter.h"
#include "kxf/General/Format.h"
#include "kxf/Drawing/GraphicsRenderer.h"

namespace kxf::WXUI
{
	// ProgressMeter
	void ProgressMeter::OnPaint(wxPaintEvent& event)
	{
		if (!m_InPulseMode)
		{
			auto renderer = m_RendererAware->GetActiveGraphicsRenderer();
			auto gc = renderer->CreateLegacyWindowPaintContext(*this);
			wxGauge::OnPaint(event);

			auto rect = m_Widget.GetRect(WidgetSizeFlag::Client);
			auto value = m_Widget.GetValuePrecentage<float>();
			gc->DrawLabel(Format("{:.2f}", value), rect, Alignment::CenterHorizontal);
		}
		else
		{
			event.Skip();
		}
	}

	bool ProgressMeter::Create(wxWindow* parent,
							   const String& label,
							   const Point& pos,
							   const Size& size
	)
	{
		if (wxGauge::Create(parent, wxID_NONE, 100, pos, size, wxGA_HORIZONTAL|wxGA_SMOOTH))
		{
			SetLabel(label);
			return true;
		}
		return false;
	}

	// wxWindow
	void ProgressMeter::SetLabel(const wxString& label)
	{
		if (!label.IsEmpty() && !IsVertical())
		{
			Bind(wxEVT_PAINT, &ProgressMeter::OnPaint, this);
		}
		else
		{
			Unbind(wxEVT_PAINT, &ProgressMeter::OnPaint, this);
		}

		ScheduleRefresh();
		wxGauge::SetLabel(label);
	}
}
