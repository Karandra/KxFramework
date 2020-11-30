#pragma once
#include "../Common.h"
#include "../Geometry.h"
#include "../Angle.h"
#include "../Color.h"
#include "../ColorDepth.h"
#include "../FontMetrics.h"
#include "../AffineMatrix.h"
#include "../GradientStops.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	class IGraphicsRenderer;
}

namespace kxf::Drawing::Private
{
	IGraphicsRenderer& GetNullGraphicsRenderer();
}
