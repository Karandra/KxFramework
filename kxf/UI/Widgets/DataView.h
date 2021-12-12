#pragma once
#include "Common.h"
#include "../IDataViewWidget.h"
#include "../IGraphicsRendererAwareWidget.h"

namespace kxf::DataView
{
	class Column;
}
namespace kxf::WXUI::DataView
{
	class View;
}

namespace kxf::Widgets
{
	class KX_API DataView: public RTTI::Implementation<DataView, Private::BasicWxWidget<DataView, WXUI::DataView::View, IDataViewWidget>, IGraphicsRendererAwareWidget>
	{
		public:
			using Column = kxf::DataView::Column;

		private:
			std::shared_ptr<IGraphicsRenderer> m_Renderer;

		public:
			DataView();
			~DataView();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IDataViewWidget
			FlagSet<WidgetStyle> GetStyle() const override;
			void SetStyle(FlagSet<WidgetStyle> style) override;

			Column* InsertColumn(size_t index, const String& title, WidgetID id = {}, FlagSet<ColumnStyle> style = ColumnStyle::Moveable|ColumnStyle::Resizeable) override;
			void RemoveColumn(DataView::Column& column) override;
			void ClearColumns() override;

			// IGraphicsRendererAwareWidget
			std::shared_ptr<IGraphicsRenderer> GetActiveGraphicsRenderer() const override;
			void SetActiveGraphicsRenderer(std::shared_ptr<IGraphicsRenderer> renderer) override
			{
				m_Renderer = std::move(renderer);
			}
	};
}
