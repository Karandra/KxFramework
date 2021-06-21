#pragma once
#include "Common.h"
#include "Geometry.h"

namespace kxf::Geometry
{
	class SizeRatio final: public Geometry::OrderedPairTemplate<SizeRatio, int>
	{
		public:
			static constexpr auto r1_1 = 1.0;
			static constexpr auto r3_2 = 3.0 / 2.0;
			static constexpr auto r4_3 = 4.0 / 3.0;
			static constexpr auto r5_4 = 5.0 / 4.0;
			static constexpr auto r16_9 = 16.0 / 9.0;
			static constexpr auto r16_10 = 16.0 / 10.0;
			static constexpr auto r21_9 = 21.0 / 9.0;
			static constexpr auto r32_9 = 32.0 / 9.0;
			static constexpr auto r256_135 = 256.0 / 135.0;

		public:
			static SizeRatio FromSystemIcon() noexcept;
			static SizeRatio FromSystemSmallIcon() noexcept;

			static constexpr SizeRatio FromWidth(int width, double ratio) noexcept
			{
				if (ratio != 0)
				{
					return {width, static_cast<int>(static_cast<double>(width) / ratio)};
				}
				return Size::UnspecifiedSize();
			}
			static constexpr SizeRatio FromHeight(int height, double ratio) noexcept
			{
				if (ratio != 0)
				{
					return {static_cast<int>(static_cast<double>(height) * ratio), height};
				}
				return Size::UnspecifiedSize();
			}

		public:
			using OrderedPairTemplate::OrderedPairTemplate;
			constexpr SizeRatio(const Size& size) noexcept
				:OrderedPairTemplate(size.GetWidth(), size.GetHeight())
			{
			}

		public:
			constexpr TValue GetWidth() const noexcept
			{
				return GetX();
			}
			constexpr SizeRatio& SetWidth(TValue width) noexcept
			{
				return SetX(width);
			}

			constexpr TValue GetHeight() const noexcept
			{
				return GetY();
			}
			constexpr SizeRatio& SetHeight(TValue height) noexcept
			{
				return SetY(height);
			}

			constexpr TValue& Width() noexcept
			{
				return X();
			}
			constexpr TValue& Height() noexcept
			{
				return Y();
			}

		public:
			constexpr bool IsFullySpecified() const noexcept
			{
				return m_X > 0 && m_Y > 0;
			}

			constexpr double GetRatio() const noexcept
			{
				if (IsFullySpecified())
				{
					return static_cast<double>(m_X) / m_Y;
				}
				return 0;
			}
			constexpr double GetReversedRatio() const noexcept
			{
				if (IsFullySpecified())
				{
					return static_cast<double>(m_Y) / m_X;
				}
				return 0;
			}

			constexpr SizeRatio ScaleMaintainRatio(Size size) const noexcept
			{
				double scale = 1.0;
				if (size.IsFullySpecified())
				{
					const double scaleX = static_cast<double>(size.GetWidth()) / GetWidth();
					const double scaleY = static_cast<double>(size.GetHeight()) / GetHeight();
					scale = std::min(scaleX, scaleY);
				}
				else if (size.GetWidth() != DefaultCoord)
				{
					scale = static_cast<double>(size.GetWidth()) / GetWidth();
				}
				else if (size.GetHeight() != DefaultCoord)
				{
					scale = static_cast<double>(size.GetHeight()) / GetHeight();
				}
				else
				{
					return Size::UnspecifiedSize();
				}
				return GetScaled(scale);
			}
			constexpr SizeRatio ScaleMaintainRatio(int width, int height) const noexcept
			{
				return ScaleMaintainRatio({width, height});
			}

		public:
			operator Size() const noexcept
			{
				return Size(m_X, m_Y);
			}
	};
}
