#pragma once
#include "../Common.h"
#include "../Geometry.h"
#include "../Angle.h"
#include "../Color.h"
#include "../ColorDepth.h"
#include "../TextExtent.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	using GraphicsFontMetrics = Drawing::BasicFontMetrics<float>;
	using GraphicsTextExtent = Drawing::BasicTextExtent<float>;
}
namespace kxf::GraphicsRenderer
{
	template<class T>
	using CoordPair = typename Geometry::OrderedPairTemplate<T, float>;
}
