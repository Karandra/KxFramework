#pragma once
#include "../Common.h"
#include <wx/defs.h>
#include <wx/font.h>
#include <wx/peninfobase.h>

namespace kxf::Drawing::Private
{
	std::optional<int> MapNativeHatchStyle(wxHatchStyle style) noexcept;
	wxHatchStyle MapNativeHatchStyle(int style) noexcept;

	wxPenStyle MapNativePenStyle(int style) noexcept;
	wxPenJoin MapNativePenJoin(int join) noexcept;

	wxHatchStyle MapHatchStyle(HatchStyle style) noexcept;
	HatchStyle MapHatchStyle(wxHatchStyle style) noexcept;

	std::optional<wxDeprecatedGUIConstants> MapDashStyle(DashStyle style) noexcept;
	DashStyle MapDashStyle(wxDeprecatedGUIConstants style) noexcept;

	wxPenJoin MapLineJoin(LineJoin join) noexcept;
	LineJoin MapLineJoin(wxPenJoin join) noexcept;

	wxPenCap MapLineCap(LineCap cap) noexcept;
	LineCap MapLineCap(wxPenCap cap) noexcept;

	std::optional<wxFloodFillStyle> MapFloodFill(FloodFill fill) noexcept;
	std::optional<wxPolygonFillMode> MapPolygonFill(PolygonFill fill) noexcept;
}

namespace kxf::Drawing::Private
{
	wxFontStyle MapFontStyle(FlagSet<FontStyle> style) noexcept;
	FlagSet<FontStyle> MapFontStyle(wxFontStyle style) noexcept;

	wxFontWeight MapFontWeight(FontWeight weight) noexcept;
	FontWeight MapFontWeight(wxFontWeight weight) noexcept;

	wxFontSymbolicSize MapFontSymbolicSize(FontSymbolicSize symbolicSize) noexcept;
	FontSymbolicSize MapFontSymbolicSize(wxFontSymbolicSize symbolicSize) noexcept;

	wxFontFamily MapFontFamily(FontFamily family) noexcept;
	FontFamily MapFontFamily(wxFontFamily family) noexcept;

	wxFontEncoding MapFontEncoding(FontEncoding encoding) noexcept;
	FontEncoding MapFontEncoding(wxFontEncoding encoding) noexcept;
}
