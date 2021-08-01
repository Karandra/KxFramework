#include "KxfPCH.h"
#include "ProgressMeter.h"
#include "WXUI/ProgressMeter.h"
#include "kxf/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// ProgressMeter
	ProgressMeter::ProgressMeter()
	{
		InitializeWxWidget();
	}
	ProgressMeter::~ProgressMeter() = default;

	// IWidget
	bool ProgressMeter::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IProgressMeter
	int ProgressMeter::GetRange() const
	{
		return Get()->GetRange();
	}
	void ProgressMeter::SetRange(int range)
	{
		Get()->SetRange(range);
	}

	int ProgressMeter::GetValue() const
	{
		return Get()->GetValue();
	}
	void ProgressMeter::SetValue(int value)
	{
		Get()->SetValue(value);
	}

	int ProgressMeter::GetStep() const
	{
		return Get()->GetStep();
	}
	void ProgressMeter::SetStep(int step)
	{
		Get()->SetValue(step);
	}

	void ProgressMeter::Pulse()
	{
		Get()->Pulse();
	}
	bool ProgressMeter::IsPulsing() const
	{
		return Get()->IsPulsing();
	}

	// IProgressMeterWidget
	String ProgressMeter::GetLabel(FlagSet<WidgetTextFlag> flags) const
	{
		if (flags.Contains(WidgetTextFlag::WithMnemonics))
		{
			return Get()->GetLabel();
		}
		else
		{
			return Get()->GetLabelText();
		}
	}
	void ProgressMeter::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
	{
		if (flags.Contains(WidgetTextFlag::WithMnemonics))
		{
			Get()->SetLabel(label);
		}
		else
		{
			Get()->SetLabelText(label);
		}
	}

	Orientation ProgressMeter::GetOrientation() const
	{
		return Get()->IsVertical() ? Orientation::Vertical : Orientation::Horizontal;
	}
	void ProgressMeter::SetOrientation(Orientation orientation)
	{
		FlagSet style = Get()->GetWindowStyle();
		style.Mod(wxGA_VERTICAL, orientation == Orientation::Vertical);
		style.Mod(wxGA_HORIZONTAL, orientation == Orientation::Horizontal);

		Get()->SetWindowStyle(*style);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> ProgressMeter::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
