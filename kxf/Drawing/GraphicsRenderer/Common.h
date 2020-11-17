#pragma once
#include "../Common.h"
#include "../Geometry.h"
#include "../Angle.h"
#include "../Color.h"
#include "../ColorDepth.h"
#include "../TextExtent.h"
#include "../AffineMatrix.h"
#include "../GradientStops.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	template<class T>
	using GraphicsCoordPair = Geometry::OrderedPairTemplate<T, float>;

	using GraphicsFontMetrics = Drawing::BasicFontMetrics<float>;
	using GraphicsTextExtent = Drawing::BasicTextExtent<float>;

	class IGraphicsRenderer;
}

namespace kxf::Drawing
{
	std::unique_ptr<IGraphicsRenderer> CreateGDIRenderer();
}
