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
	KX_API std::shared_ptr<IGraphicsRenderer> GetGDIRenderer();
	KX_API std::shared_ptr<IGraphicsRenderer> GetGDIPlusRenderer();
	KX_API std::shared_ptr<IGraphicsRenderer> GetDirect2DRenderer();
	KX_API std::shared_ptr<IGraphicsRenderer> GetCairoRenderer();

	KX_API std::shared_ptr<IGraphicsRenderer> GetDefaultRenderer();
	KX_API void SetDefaultRenderer(std::shared_ptr<IGraphicsRenderer> renderer);

	KX_API size_t EnumAvailableRenderers(std::function<bool(std::shared_ptr<IGraphicsRenderer>)> func);
	KX_API std::shared_ptr<IGraphicsRenderer> GetRendererByName(const String& name);
}
