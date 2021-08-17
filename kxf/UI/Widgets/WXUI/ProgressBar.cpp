#include "KxfPCH.h"
#include "ProgressBar.h"
#include "kxf/General/Format.h"
#include "kxf/Drawing/GraphicsRenderer.h"

namespace kxf::WXUI
{
	// ProgressBar
	void ProgressBar::OnPaint(wxPaintEvent& event)
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

	bool ProgressBar::Create(wxWindow* parent,
							   const String& label,
							   const Point& pos,
							   const Size& size
	)
	{
		if (wxGauge::Create(parent, wxID_NONE, 100, pos, size, wxGA_HORIZONTAL|wxGA_SMOOTH))
		{
			SetLabel(label);

			return m_Widget.QueryInterface(m_RendererAware);
		}
		return false;
	}

	// wxWindow
	void ProgressBar::SetLabel(const wxString& label)
	{
		if (!label.IsEmpty() && !IsVertical())
		{
			Bind(wxEVT_PAINT, &ProgressBar::OnPaint, this);
		}
		else
		{
			Unbind(wxEVT_PAINT, &ProgressBar::OnPaint, this);
		}

		ScheduleRefresh();
		wxGauge::SetLabel(label);
	}
}
