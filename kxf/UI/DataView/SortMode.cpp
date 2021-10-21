#include "KxfPCH.h"
#include "SortMode.h"
#include "../IDataViewWidget.h"

namespace kxf::DataView
{
	SortMode::SortMode(const IDataViewWidget& widget) noexcept
	{
		if (widget.IsMultiColumnSortUsed())
		{
			m_SortWhat = SortWhat::MultiColumn;
		}
		else if (auto column = widget.GetSortingColumn())
		{
			m_SortWhat = SortWhat::SingleColumn;
			m_SortOrder = column->GetSortOrder();
			m_SortingColumn = std::move(column);
		}
	}
	SortMode::SortMode(std::shared_ptr<IDataViewColumn> column) noexcept
		:m_SortingColumn(column), m_SortWhat(SortWhat::SingleColumn), m_SortOrder(column->GetSortOrder())
	{
	}
}
