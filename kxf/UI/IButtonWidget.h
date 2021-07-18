#pragma once
#include "Common.h"
#include "IWidget.h"

namespace kxf
{
	class KX_API IButtonWidget: public RTTI::ExtendInterface<IButtonWidget, IWidget>
	{
		KxRTTI_DeclareIID(IButtonWidget, {0x5c9bb34e, 0x667d, 0x4051, {0x8b, 0x7b, 0xc9, 0x65, 0x58, 0xca, 0x13, 0xf5}});

		public:
			virtual String GetButtonLabel(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetButtonLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) = 0;

			virtual BitmapImage GetButtonIcon() const = 0;
			virtual void SetButtonIcon(const BitmapImage& icon, FlagSet<Direction> direction = {}) = 0;
			virtual void SetStdButtonIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction = {}) = 0;

			virtual bool IsDefaultButton() const = 0;
			virtual std::shared_ptr<IButtonWidget> SetDefaultButton() = 0;

			virtual bool IsDropdownEnabled() const = 0;
			virtual void SetDropdownEnbled(bool enabled = true) = 0;
	};
}
