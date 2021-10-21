#pragma once
#include "Common.h"
#include "../IDataViewWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::WXUI::DataView
{
	class View;
}

namespace kxf::Widgets
{
	class KX_API DataView: public RTTI::Implementation<DataView, Private::BasicWxWidget<DataView, WXUI::DataView::View, IDataViewWidget>, IGraphicsRendererAwareWidget>
	{
		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			DataView();
			~DataView();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IDataViewWidget
			FlagSet<Style> GetStyle() const override;
			void SetStyle(FlagSet<Style> style) override;

			std::shared_ptr<IDataViewColumn> CreateColumn(const String& title, WidgetID id = {}, FlagSet<ColumnStyle> style = ColumnStyle::Moveable|ColumnStyle::Resizeable) const override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
