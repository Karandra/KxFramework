#pragma once
#include "Common.h"

namespace kxf
{
	class IDataViewWidget;
	class IDataViewColumn;
}

namespace kxf::DataView
{
	class KX_API SortMode final
	{
		private:
			enum class SortWhat
			{
				None = -1,
				SingleColumn = 0,
				MultiColumn = 1
			};

		private:
			std::shared_ptr<IDataViewColumn> m_SortingColumn;
			SortWhat m_SortWhat = SortWhat::None;
			SortOrder m_SortOrder = SortOrder::None;

		public:
			SortMode() noexcept = default;
			SortMode(const IDataViewWidget& widget) noexcept;
			SortMode(std::shared_ptr<IDataViewColumn> column) noexcept;

		public:
			bool IsSortRequired() const noexcept
			{
				return m_SortWhat != SortWhat::None;
			}
			bool IsSingleColumn() const noexcept
			{
				return m_SortWhat == SortWhat::SingleColumn;
			}
			bool IsMultiColumn() const noexcept
			{
				return m_SortWhat == SortWhat::MultiColumn;
			}

			const IDataViewColumn* GetSortingColumn() const noexcept
			{
				return m_SortingColumn.get();
			}
			SortOrder GetSortOrder() const noexcept
			{
				return m_SortOrder;
			}

		public:
			bool operator==(const SortMode&) const noexcept = default;
	};
}
