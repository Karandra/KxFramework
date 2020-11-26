#include "stdafx.h"
#include "GraphicsRenderer.h"
#include "GDIRenderer/GDIGraphicsRenderer.h"
#include "WxRenderer/WxGraphicsRenderer.h"

namespace
{
	std::shared_ptr<kxf::IGraphicsRenderer> GetWxRenderer(wxGraphicsRenderer* renderer)
	{
		if (renderer)
		{
			return std::make_shared<kxf::WxGraphicsRenderer>(*renderer);
		}
		return nullptr;
	}
}

namespace kxf::Drawing
{
	std::shared_ptr<IGraphicsRenderer> GetGDIRenderer()
	{
		static auto renderer = std::make_shared<GDIGraphicsRenderer>();
		return renderer;
	}
	std::shared_ptr<IGraphicsRenderer> GetGDIPlusRenderer()
	{
		static auto renderer = GetWxRenderer(wxGraphicsRenderer::GetGDIPlusRenderer());
		return renderer;
	}
	std::shared_ptr<IGraphicsRenderer> GetDirect2DRenderer()
	{
		static auto renderer = GetWxRenderer(wxGraphicsRenderer::GetDirect2DRenderer());
		return renderer;
	}
	std::shared_ptr<IGraphicsRenderer> GetCairoRenderer()
	{
		static auto renderer = GetWxRenderer(wxGraphicsRenderer::GetCairoRenderer());
		return renderer;
	}

	std::shared_ptr<IGraphicsRenderer> GetDefaultRenderer()
	{
		static auto renderer = GetWxRenderer(wxGraphicsRenderer::GetDefaultRenderer());

		if (renderer)
		{
			return renderer;
		}
		return GetGDIRenderer();
	}
}
