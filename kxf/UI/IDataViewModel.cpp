#include "KxfPCH.h"
#include "IDataViewModel.h"
#include "DataView/Node.h"

namespace kxf
{
	std::shared_ptr<IDataViewCellRenderer> IDataViewModel::GetCellRenderer(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellRenderer(column);
	}
	std::shared_ptr<IDataViewCellEditor> IDataViewModel::GetCellEditor(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellEditor(column);
	}

	Any IDataViewModel::GetCellDisplayValue(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellDisplayValue(column);
	}
	Any IDataViewModel::GetCellValue(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellValue(column);
	}
	bool IDataViewModel::SetCellValue(DataView::Node& node, DataView::Column& column, Any value)
	{
		return node.GetItem()->SetCellValue(column, std::move(value));
	}

	DataView::ToolTip IDataViewModel::GetCellToolTip(const DataView::Node& node, const DataView::Column& column) const
	{
		return node.GetItem()->GetCellToolTip(column);
	}
	kxf::DataView::CellAttributes IDataViewModel::GetCellAttributes(const DataView::Node& node, const DataView::Column& column, const DataView::CellState& cellState) const
	{
		return node.GetItem()->GetCellAttributes(column, cellState);
	}
}
