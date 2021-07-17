#pragma once
#include "Common.h"
#include "../IButtonWidget.h"

namespace kxf::WXUI
{
	class Button;
}

namespace kxf::Widgets
{
	class KX_API Button: public RTTI::Implementation<Button, Private::BasicWxWidget<Button, WXUI::Button, IButtonWidget>>
	{
		public:
			Button();
			~Button();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& text = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IButtonWidget
			bool IsDefaultButton() const override;
			std::shared_ptr<IButtonWidget> SetDefaultButton() override;

			BitmapImage GetButtonIcon() const override;
			void SetButtonIcon(const BitmapImage& icon, FlagSet<Direction> direction = {}) override;
			void SetStdButtonIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction = {}) override;

			bool IsDropdownEnabled() const override;
			void SetDropdownEnbled(bool enabled = true) override;
	};
}
