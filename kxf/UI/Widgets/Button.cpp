#include "KxfPCH.h"
#include "Button.h"
#include "WXUI/Button.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/Drawing/ImageBundle.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"

namespace kxf::Widgets
{
	// Button
	Button::Button()
	{
		InitializeWxWidget();
	}
	Button::~Button() = default;

	// IWidget
	bool Button::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IButtonWidget
	String Button::GetLabel(FlagSet<WidgetTextFlag> flags) const
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
	void Button::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
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

	BitmapImage Button::GetIcon() const
	{
		return GDIBitmap(Get()->GetBitmap());
	}
	void Button::SetIcon(const BitmapImage& icon, FlagSet<Direction> direction)
	{
		Get()->SetBitmap(icon.ToGDIBitmap().ToWxBitmap());
	}
	void Button::SetStdIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction)
	{
		if (stdIcon.Contains(StdIcon::Authentication))
		{
			DynamicLibrary library("ImageRes.dll", DynamicLibraryFlag::Resource);
			if (library)
			{
				// 78 is the index of UAC shield icon
				if (ImageBundle bundle = library.GetIconBundleResource("78"))
				{
					if (BitmapImage image = bundle.GetImage(Size(GetSize()), ImageBundleFlag::SystemSize|ImageBundleFlag::NearestLarger))
					{
						Button::SetIcon(image, direction);
						return;
					}
				}
			}
		}

		Button::SetIcon({}, direction);
	}

	bool Button::IsDefault() const
	{
		return Get()->IsDefault();
	}
	std::shared_ptr<IButtonWidget> Button::SetDefault()
	{
		if (auto button = Get()->SetDefault())
		{
			if (auto widget = Private::FindByWXObject(*button))
			{
				return widget->QueryInterface<IButtonWidget>();
			}
		}
		return {};
	}

	bool Button::IsDropdownEnabled() const
	{
		return Get()->IsDropdownEnabled();
	}
	void Button::SetDropdownEnbled(bool enabled)
	{
		Get()->SetDropdownEnbled(enabled);
	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> Button::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
