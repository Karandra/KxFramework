#pragma once
#include "Common.h"
#include <wx/dc.h>

namespace kxf::Drawing
{
	template<class TValue>
	struct BasicFontMetrics final
	{
		public:
			TValue Height = 0; // Total character height
			TValue Ascent = 0; // Part of the height above the baseline
			TValue Descent = 0; // Part of the height below the baseline
			TValue AverageWidth = 0; // Average font width or "x-width"
			TValue InternalLeading = 0; // Intra-line spacing
			TValue ExternalLeading = 0; // Inter-line spacing

		public:
			constexpr BasicFontMetrics() noexcept = default;
			constexpr BasicFontMetrics(const wxFontMetrics& other) noexcept
				:Height(other.height), Ascent(other.ascent), Descent(other.descent),
				AverageWidth(other.averageWidth), InternalLeading(other.internalLeading), ExternalLeading(other.externalLeading)
			{
			}
			constexpr BasicFontMetrics(TValue height, TValue ascent, TValue descent, TValue averageWidth, TValue internalLeading, TValue externalLeading) noexcept
				:Height(height), Ascent(ascent), Descent(descent),
				AverageWidth(averageWidth), InternalLeading(internalLeading), ExternalLeading(externalLeading)
			{
			}

			template<class T>
			constexpr BasicFontMetrics(const BasicFontMetrics<T>& other) noexcept
				:Height(other.Height), Ascent(other.Ascent), Descent(other.Descent),
				AverageWidth(other.AverageWidth), InternalLeading(other.InternalLeading), ExternalLeading(other.ExternalLeading)
			{
			}

		public:
			operator wxFontMetrics() const noexcept
			{
				wxFontMetrics fontMetrics;
				fontMetrics.height = Height;
				fontMetrics.ascent = Ascent;
				fontMetrics.descent = Descent;
				fontMetrics.averageWidth = AverageWidth;
				fontMetrics.internalLeading = InternalLeading;
				fontMetrics.externalLeading = ExternalLeading;
			}

	};
}
