#pragma once
#include "Common.h"
#include <wx/dc.h>

namespace kxf::Drawing
{
	template<class T>
	struct BasicFontMetrics final
	{
		public:
			T Height = 0; // Total character height
			T Ascent = 0; // Part of the height above the baseline
			T Descent = 0; // Part of the height below the baseline
			T AverageWidth = 0; // Average font width or "x-width"
			T InternalLeading = 0; // Intra-line spacing
			T ExternalLeading = 0; // Inter-line spacing

		public:
			constexpr BasicFontMetrics() noexcept = default;
			constexpr BasicFontMetrics(T height, T ascent, T descent, T averageWidth, T internalLeading, T externalLeading) noexcept
				:Height(height), Ascent(ascent), Descent(descent),
				AverageWidth(averageWidth), InternalLeading(internalLeading), ExternalLeading(externalLeading)
			{
			}
			constexpr BasicFontMetrics(const wxFontMetrics& other) noexcept
				:Height(other.height), Ascent(other.ascent), Descent(other.descent),
				AverageWidth(other.averageWidth), InternalLeading(other.internalLeading), ExternalLeading(other.externalLeading)
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
