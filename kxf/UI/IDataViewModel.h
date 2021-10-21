#pragma once
#include "Common.h"
#include "IDataViewItem.h"
#include "DataView/Node.h"
#include "kxf/General/Any.h"

namespace kxf::DataView
{
	class Node;
	class Column;
	class CellEditor;
	class CellRenderer;

	class ToolTip;
	class SortMode;
	class CellState;
	class CellAttributes;
}

namespace kxf
{
	class KX_API IDataViewModel: public RTTI::Interface<IDataViewModel>
	{
		KxRTTI_DeclareIID(IDataViewModel, {0x96657379, 0x220e, 0x4df1, {0xa8, 0x4d, 0x9d, 0x66, 0xd9, 0x4e, 0xa, 0xa3}});

		public:
			virtual void OnSortChildren(const DataView::SortMode& sortMode)
			{
			}

			virtual size_t GetChildrenCount() const = 0;
			virtual Enumerator<std::shared_ptr<IDataViewItem>> EnumChildren() const = 0;

		public:
			virtual std::shared_ptr<DataView::CellRenderer> GetCellRenderer(const DataView::Node& node, const DataView::Column& column) const
			{
				return node.GetItem()->GetCellRenderer(column);
			}
			virtual std::shared_ptr<DataView::CellEditor> GetCellEditor(const DataView::Node& node, const DataView::Column& column) const
			{
				return node.GetItem()->GetCellEditor(column);
			}

			virtual Any GetCellDisplayValue(const DataView::Node& node, const DataView::Column& column) const
			{
				return node.GetItem()->GetCellDisplayValue(column);
			}
			virtual Any GetCellValue(const DataView::Node& node, const DataView::Column& column) const
			{
				return node.GetItem()->GetCellValue(column);
			}
			virtual bool SetCellValue(DataView::Node& node, DataView::Column& column, Any value)
			{
				return node.GetItem()->SetCellValue(column, std::move(value));
			}

			virtual DataView::ToolTip GetCellToolTip(const DataView::Node& node, const DataView::Column& column) const
			{
				return node.GetItem()->GetCellToolTip(column);
			}
			virtual DataView::CellAttributes GetCellAttributes(const DataView::Node& node, const DataView::Column& column, const DataView::CellState& cellState) const
			{
				return node.GetItem()->GetCellAttributes(column, cellState);
			}
	};
}
