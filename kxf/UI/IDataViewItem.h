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
			// Internal state
			virtual void OnAttach(DataView::Node& node)
			{
			}
			virtual void OnDetach(DataView::Node& node)
			{
			}

			virtual bool OnExpand(DataView::Node& node)
			{
				return true;
			}
			virtual bool OnCollapse(DataView::Node& node)
			{
				return true;
			}

			virtual bool OnSelect(DataView::Node& node)
			{
				return true;
			}
			virtual bool OnActivate(DataView::Node& node)
			{
				return true;
			}

		public:
			// Sorting and children
			virtual void OnSortChildren(const DataView::SortMode& sortMode)
			{
			}
			virtual std::partial_ordering Compare(const IDataViewItem& other, const DataView::SortMode& sortMode) const
			{
				return this <=> &other;
			}

			virtual size_t GetChildrenCount() const
			{
				return 0;
			}
			virtual std::shared_ptr<IDataViewItem> GetChildItem(size_t index) const
			{
				return {};
			}

		public:
			// Visuals and editing
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
				return Compare(other, {}) == 0;
			}
			auto operator<=>(const IDataViewItem& other) const
			{
				return Compare(other, {});
			}
	};
}
