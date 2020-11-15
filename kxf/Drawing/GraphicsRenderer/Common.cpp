#include "stdafx.h"
#include "Common.h"
#include "../GDIRenderer/GDIGraphicsRenderer.h"

namespace kxf::Drawing
{
	std::unique_ptr<IGraphicsRenderer> CreateGDIRenderer()
	{
		return std::make_unique<GDIGraphicsRenderer>();
	}
}
