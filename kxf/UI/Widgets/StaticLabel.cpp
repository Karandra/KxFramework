#include "KxfPCH.h"
#include "StaticLabel.h"
#include "WXUI/StaticLabel.h"
#include "kxf/Drawing/ArtProvider.h"
#include "kxf/Drawing/GraphicsRenderer.h"

namespace kxf::Widgets
{
	// Button
	StaticLabel::StaticLabel()
	{
		InitializeWxWidget();
	}
	StaticLabel::~StaticLabel() = default;

	// IWidget
	bool StaticLabel::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IButtonWidget
	String StaticLabel::GetLabel(FlagSet<WidgetTextFlag> flags) const
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
	void StaticLabel::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
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

	BitmapImage StaticLabel::GetIcon() const
	{
		return Get()->GetBitmap();
	}
	void StaticLabel::SetIcon(const BitmapImage& icon, FlagSet<Direction> direction)
	{
		Get()->SetBitmap(icon);
	}
	void StaticLabel::SetStdIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction)
	{
		if (stdIcon.Contains(StdIcon::Authentication))
		{
			StaticLabel::SetIcon(ArtProvider::GetMessageBoxResource(StdIcon::Authentication), direction);
		}
		else
		{
			StaticLabel::SetIcon({}, direction);
		}
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> StaticLabel::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
