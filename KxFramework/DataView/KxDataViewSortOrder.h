#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/DataView/KxDataViewItem.h"
class KX_API KxDataViewColumn;

// A class storing the definition of sort order used, as a column index and
// sort direction by this column.
// Notice that the sort order may be invalid, meaning that items shouldn't be sorted.
class KX_API KxDataViewSortOrder
{
	private:
		enum class SortWhat
		{
			// Sort specified column
			Column = 0,

			// Don't sort at all.
			None = 1,

			// Sort using the model default sort order.
			Default = 2,
		};

	public:
		static KxDataViewSortOrder UseColumn(KxDataViewColumn* column, bool isAscending = true)
		{
			return KxDataViewSortOrder(column, isAscending);
		}
		static KxDataViewSortOrder UseDefault(bool isAscending = true)
		{
			return KxDataViewSortOrder(SortWhat::Default, isAscending);
		}
		static KxDataViewSortOrder UseNone(bool isAscending = true)
		{
			return KxDataViewSortOrder(SortWhat::None, isAscending);
		}

	private:
		SortWhat m_SortWhat = SortWhat::None;
		bool m_IsAscending = true;

	private:
		KxDataViewSortOrder(SortWhat sortWhat = SortWhat::None, bool isAscending = true)
			:m_SortWhat(sortWhat), m_IsAscending(isAscending)
		{
		}
		KxDataViewSortOrder(KxDataViewColumn* column, bool isAscending = true)
			:m_SortWhat(SortWhat::Column), m_IsAscending(isAscending)
		{
		}

	public:
		bool IsAscending() const
		{
			return m_IsAscending;
		}

		bool IsNone() const
		{
			return m_SortWhat == SortWhat::None;
		}
		bool IsDefault() const
		{
			return m_SortWhat == SortWhat::Default;
		}
		bool IsColumn() const
		{
			return m_SortWhat == SortWhat::Column;
		}

	public:
		bool operator==(const KxDataViewSortOrder& other) const
		{
			return m_SortWhat == other.m_SortWhat && m_IsAscending == other.m_IsAscending;
		}
		bool operator!=(const KxDataViewSortOrder& other) const
		{
			return !(*this == other);
		}
};
