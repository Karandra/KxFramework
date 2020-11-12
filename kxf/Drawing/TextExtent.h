#pragma once
#include "Common.h"
#include "Geometry.h"
#include "FontMetrics.h"

namespace kxf::Drawing
{
	template<class T>
	class BasicTextExtent final
	{
		public:
			using TSize = typename Geometry::BasicSize<T>;
			using TFontMetrics = typename BasicFontMetrics<T>;

		private:
			TSize m_Extent;
			TFontMetrics m_FontMetrics;

		public:
			constexpr BasicTextExtent(TSize extent, TFontMetrics metrics = {}) noexcept
				:m_Extent(std::move(extent)), m_FontMetrics(std::move(metrics))
			{
				if (m_Extent.GetHeight() <= 0)
				{
					m_Extent.SetHeight(m_FontMetrics.Height);
				}
				if (m_FontMetrics.Height <= 0)
				{
					m_FontMetrics.Height = m_Extent.GetHeight();
				}
			}

		public:
			constexpr TSize GetExtent() const noexcept
			{
				return m_Extent;
			}
			constexpr TFontMetrics GetFontMetrics() const noexcept
			{
				return m_FontMetrics;
			}
	};
}
