#pragma once
#include "GraphicsRenderer/Common.h"

#include "GraphicsRenderer/IGraphicsObject.h"
#include "GraphicsRenderer/IGraphicsRenderer.h"
#include "GraphicsRenderer/IGraphicsContext.h"
#include "GraphicsRenderer/IGraphicsTexture.h"
#include "GraphicsRenderer/IGraphicsPath.h"
#include "GraphicsRenderer/IGraphicsFont.h"
#include "GraphicsRenderer/IGraphicsBrush.h"
#include "GraphicsRenderer/IGraphicsPen.h"

#include "GraphicsRenderer/GraphicsAction.h"

namespace kxf::Drawing
{
	std::shared_ptr<IGraphicsRenderer> GetGDIRenderer();
	std::shared_ptr<IGraphicsRenderer> GetGDIPlusRenderer();
	std::shared_ptr<IGraphicsRenderer> GetDirect2DRenderer();
	std::shared_ptr<IGraphicsRenderer> GetCairoRenderer();

	std::shared_ptr<IGraphicsRenderer> GetDefaultRenderer();
	void SetDefaultRenderer(std::shared_ptr<IGraphicsRenderer> renderer);
}
