#pragma once
#include "Common.h"
#include "IDataViewModel.h"
#include "DataView/Common.h"
#include "DataView/ToolTip.h"
#include "DataView/CellAttributes.h"
#include "kxf/General/Enumerator.h"

namespace kxf
{
	class KX_API IDataViewItem: public RTTI::Interface<IDataViewItem>
	{
		KxRTTI_DeclareIID(IDataViewItem, {0xb36ca63f, 0x4c51, 0x4409, {0x8b, 0x4c, 0xdf, 0xa5, 0x95, 0x59, 0x14, 0xef}});

		friend class DataView::Node;

		public:
			static constexpr auto npos = std::numeric_limits<size_t>::max();

		protected:
			virtual bool IsSameAs(const IDataViewItem& other) const
			{
				return this == &other;
			}

			virtual void OnAttach(DataView::Node& node)
			{
			}
			virtual void OnDetach()
			{
			}

			virtual bool OnExpand() const
			{
				return true;
			}
			virtual bool OnCollapse() const
			{
				return true;
			}

		public:
			virtual void OnSortChildren(const DataView::SortMode& sortMode)
			{
			}

			virtual bool HasChildren() const
			{
				return GetChildrenCount() != 0;
			}
			virtual size_t GetChildrenCount() const
			{
				return EnumChildren().CalcTotalCount();
			}
			virtual Enumerator<std::shared_ptr<IDataViewItem>> EnumChildren() const
			{
				return {};
			}
			virtual bool IsExpanded() const
			{
				return false;
			}

		public:
			virtual std::shared_ptr<DataView::CellRenderer> GetCellRenderer(const DataView::Column& column) const
			{
				return nullptr;
			}
			virtual std::shared_ptr<DataView::CellEditor> GetCellEditor(const DataView::Column& column) const
			{
				return nullptr;
			}

			virtual Any GetCellValue(const DataView::Column& column) const = 0;
			virtual Any GetCellDisplayValue(const DataView::Column& column) const
			{
				return GetCellValue(column);
			}
			virtual bool SetCellValue(DataView::Column& column, Any value)
			{
				return false;
			}

			virtual DataView::ToolTip GetCellToolTip(const DataView::Column& column) const
			{
				return {};
			}
			virtual DataView::CellAttributes GetCellAttributes(const DataView::Column& column, const DataView::CellState& cellState) const
			{
				return {};
			}

		public:
			bool operator==(const IDataViewItem& other) const
			{
				return IsSameAs(other);
			}
	};
}
