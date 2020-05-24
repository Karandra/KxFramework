#pragma once
#include "Common.h"

namespace kxf::UI::DataView
{
	class Column;
}

namespace kxf::UI::DataView
{
	// A class storing the definition of sort order used, as a column index and sort direction by this column.
	// Notice that the sort order may be invalid, meaning that items shouldn't be sorted.
	class SortOrder final
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
			static SortOrder UseColumn(const Column& column, bool isAscending = true) noexcept
			{
				return SortOrder(column, isAscending);
			}
			static SortOrder UseDefault(bool isAscending = true) noexcept
			{
				return SortOrder(SortWhat::Default, isAscending);
			}
			static SortOrder UseNone(bool isAscending = true) noexcept
			{
				return SortOrder(SortWhat::None, isAscending);
			}

		private:
			SortWhat m_SortWhat = SortWhat::None;
			bool m_IsAscending = true;

		private:
			SortOrder(SortWhat sortWhat = SortWhat::None, bool isAscending = true) noexcept
				:m_SortWhat(sortWhat), m_IsAscending(isAscending)
			{
			}
			SortOrder(const Column& column, bool isAscending = true) noexcept
				:m_SortWhat(SortWhat::Column), m_IsAscending(isAscending)
			{
			}

		public:
			bool IsAscending() const noexcept
			{
				return m_IsAscending;
			}

			bool IsNone() const noexcept
			{
				return m_SortWhat == SortWhat::None;
			}
			bool IsDefault() const noexcept
			{
				return m_SortWhat == SortWhat::Default;
			}
			bool IsColumn() const noexcept
			{
				return m_SortWhat == SortWhat::Column;
			}

		public:
			bool operator==(const SortOrder& other) const noexcept
			{
				return m_SortWhat == other.m_SortWhat && m_IsAscending == other.m_IsAscending;
			}
			bool operator!=(const SortOrder& other) const noexcept
			{
				return !(*this == other);
			}
	};
}
