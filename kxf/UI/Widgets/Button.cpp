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
	bool Button::CreateWidget(std::shared_ptr<IWidget> parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}

	// IButtonWidget
	String Button::GetButtonLabel(FlagSet<WidgetTextFlag> flags) const
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
	void Button::SetButtonLabel(const String& label, FlagSet<WidgetTextFlag> flags)
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

	BitmapImage Button::GetButtonIcon() const
	{
		return GDIBitmap(Get()->GetBitmap());
	}
	void Button::SetButtonIcon(const BitmapImage& icon, FlagSet<Direction> direction)
	{
		Get()->SetBitmap(icon.ToGDIBitmap().ToWxBitmap());
	}
	void Button::SetStdButtonIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction)
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
						Button::SetButtonIcon(image, direction);
						return;
					}
				}
			}
		}

		Button::SetButtonIcon({}, direction);
	}

	bool Button::IsDefaultButton() const
	{
		return Get()->IsDefaultButton();
	}
	std::shared_ptr<IButtonWidget> Button::SetDefaultButton()
	{
		if (auto button = Get()->SetDefaultButton())
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
