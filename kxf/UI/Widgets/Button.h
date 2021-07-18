#pragma once
#include "Common.h"
#include "../IButtonWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class Button;
}

namespace kxf::Widgets
{
	class KX_API Button: public RTTI::Implementation<Button, Private::BasicWxWidget<Button, WXUI::Button, IButtonWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			Button();
			~Button();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& text = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IButtonWidget
			String GetButtonLabel(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetButtonLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override;

			BitmapImage GetButtonIcon() const override;
			void SetButtonIcon(const BitmapImage& icon, FlagSet<Direction> direction = {}) override;
			void SetStdButtonIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction = {}) override;

			bool IsDefaultButton() const override;
			std::shared_ptr<IButtonWidget> SetDefaultButton() override;

			bool IsDropdownEnabled() const override;
			void SetDropdownEnbled(bool enabled = true) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
