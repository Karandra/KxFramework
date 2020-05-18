#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"

namespace KxFramework::UI::DataView
{
	class Column;
}

namespace KxFramework::UI::DataView
{
	// A class storing the definition of sort order used, as a column index and sort direction by this column.
	// Notice that the sort order may be invalid, meaning that items shouldn't be sorted.
	class SortOrder final
	{
		private:
			enum class SortWhat: uint8_t
			{
				// Sort specified column
				Column = 0,

				// Don't sort at all.
				None = 1,

				// Sort using the model default sort order.
				Default = 2,
			};

		public:
			static SortOrder UseColumn(const Column& column, bool isAscending = true)
			{
				return SortOrder(column, isAscending);
			}
			static SortOrder UseDefault(bool isAscending = true)
			{
				return SortOrder(SortWhat::Default, isAscending);
			}
			static SortOrder UseNone(bool isAscending = true)
			{
				return SortOrder(SortWhat::None, isAscending);
			}

		private:
			SortWhat m_SortWhat = SortWhat::None;
			bool m_IsAscending = true;

		private:
			SortOrder(SortWhat sortWhat = SortWhat::None, bool isAscending = true)
				:m_SortWhat(sortWhat), m_IsAscending(isAscending)
			{
			}
			SortOrder(const Column& column, bool isAscending = true)
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
			bool operator==(const SortOrder& other) const
			{
				return m_SortWhat == other.m_SortWhat && m_IsAscending == other.m_IsAscending;
			}
			bool operator!=(const SortOrder& other) const
			{
				return !(*this == other);
			}
	};
}
