#include "KxfPCH.h"
#include "DataView.h"
#include "WXUI/DataView/View.h"
#include "WXUI/DataView/MainWindow.h"
#include "kxf/Drawing/ArtProvider.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"

namespace kxf::Widgets
{
	// DataView
	DataView::DataView()
	{
		InitializeWxWidget();
	}
	DataView::~DataView() = default;

	// IWidget
	bool DataView::CreateWidget(std::shared_ptr<IWidget> parent, const String& label, Point pos, Size size)
	{
		if (parent)
		{
			return Get()->Create(parent->GetWxWindow(), label, pos, size);
		}
		return false;
	}

	// IDataViewWidget
	FlagSet<IDataViewWidget::WidgetStyle> DataView::GetStyle() const
	{
		return Get()->GetStyle();
	}
	void DataView::SetStyle(FlagSet<WidgetStyle> style)
	{
		Get()->SetStyle(style);
	}

	DataView::Column* DataView::InsertColumn(size_t index, const String& title, WidgetID id, FlagSet<ColumnStyle> style)
	{
		return nullptr;
	}
	void DataView::RemoveColumn(DataView::Column& column)
	{

	}
	void DataView::ClearColumns()
	{

	}

	// IGraphicsRendererAwareWidget
	std::shared_ptr<IGraphicsRenderer> DataView::GetActiveGraphicsRenderer() const
	{
		if (!m_Renderer)
		{
			return Drawing::GetDefaultRenderer();
		}
		return m_Renderer;
	}
}
