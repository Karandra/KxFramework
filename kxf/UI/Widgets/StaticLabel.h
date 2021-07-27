#pragma once
#include "Common.h"
#include "../IStaticLabel.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI
{
	class StaticLabel;
}

namespace kxf::Widgets
{
	class KX_API StaticLabel: public RTTI::Implementation<StaticLabel, Private::BasicWxWidget<StaticLabel, WXUI::StaticLabel, IStaticLabel>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			StaticLabel();
			~StaticLabel();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IStaticLabel
			String GetLabel(FlagSet<WidgetTextFlag> flags = {}) const override;
			void SetLabel(const String& label, FlagSet<WidgetTextFlag> flags = {}) override;

			BitmapImage GetIcon() const override;
			void SetIcon(const BitmapImage& icon, FlagSet<Direction> direction = {}) override;
			void SetStdIcon(FlagSet<StdIcon> stdIcon, FlagSet<Direction> direction = {}) override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
