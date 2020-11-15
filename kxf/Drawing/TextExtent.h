#pragma once
#include "Common.h"
#include "Geometry.h"
#include "FontMetrics.h"

namespace kxf::Drawing
{
	template<class TValue>
	class BasicTextExtent final
	{
		public:
			using TSize = Geometry::BasicSize<TValue>;
			using TFontMetrics = BasicFontMetrics<TValue>;

		private:
			TSize m_Extent;
			TFontMetrics m_FontMetrics;

		private:
			void Initialize()
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
			constexpr BasicTextExtent(TSize extent, TFontMetrics metrics = {}) noexcept
				:m_Extent(std::move(extent)), m_FontMetrics(std::move(metrics))
			{
				Initialize();
			}

			template<class T>
			constexpr BasicTextExtent(const BasicTextExtent<T>& other) noexcept
				:m_Extent(std::move(other.GetExtent())), m_FontMetrics(std::move(other.GetFontMetrics()))
			{
				Initialize();
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
