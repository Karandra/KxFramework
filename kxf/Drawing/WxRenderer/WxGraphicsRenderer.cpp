#include "stdafx.h"
#include "WxGraphicsRenderer.h"
#include "WxGraphicsContext.h"
#include "WxGraphicsTexture.h"
#include "WxGraphicsBrush.h"
#include "WxGraphicsPen.h"
#include "WxGraphicsFont.h"
#include "WxGraphicsPath.h"
#include <wx/graphics.h>

namespace kxf
{
	// WxGraphicsRenderer
	WxGraphicsRenderer::WxGraphicsRenderer(wxGraphicsRenderer& renderer)
		:m_Renderer(renderer)
	{
		if (&renderer == wxGraphicsRenderer::GetGDIPlusRenderer())
		{
			m_Type = Type::GDIPlus;
		}
		else if (&renderer == wxGraphicsRenderer::GetDirect2DRenderer())
		{
			m_Type = Type::Direct2D;
		}
		else if (&renderer == wxGraphicsRenderer::GetCairoRenderer())
		{
			m_Type = Type::Cairo;
		}
	}

	// IGraphicsRenderer
	String WxGraphicsRenderer::GetName() const
	{
		return m_Renderer.GetName();
	}
	Version WxGraphicsRenderer::GetVersion() const
	{
		int major = 0;
		int minor = 0;
		int micro = 0;
		m_Renderer.GetVersion(&major, &minor, &micro);

		return {major, minor, micro};
	}

	std::unique_ptr<IGraphicsContext> WxGraphicsRenderer::CreateContext(std::shared_ptr<IGraphicsTexture> texture)
	{
		if (texture)
		{
			return std::make_unique<WxGraphicsMemoryContext>(*this, std::move(texture));
		}
		return nullptr;
	}
	std::unique_ptr<IGraphicsContext> WxGraphicsRenderer::CreateGDIContext(wxDC& dc)
	{
		if (dc.IsOk())
		{
			return std::make_unique<WxGraphicsGDIContext>(*this, dc);
		}
		return nullptr;
	}
	std::unique_ptr<IGraphicsContext> WxGraphicsRenderer::CreateWindowContext(wxWindow& window)
	{
		return std::make_unique<WxGraphicsWindowContext>(*this, window);
	}
	std::unique_ptr<IGraphicsContext> WxGraphicsRenderer::CreateWindowClientContext(wxWindow& window)
	{
		return std::make_unique<WxGraphicsWindowClientContext>(*this, window);
	}
	std::unique_ptr<IGraphicsContext> WxGraphicsRenderer::CreateWindowPaintContext(wxWindow& window)
	{
		return std::make_unique<WxGraphicsPaintContext>(*this, window);
	}
	std::unique_ptr<IGraphicsContext> WxGraphicsRenderer::CreateMeasuringContext()
	{
		return std::make_unique<WxGraphicsMeasuringContext>(*this);
	}

	// Pen and brush functions
	std::shared_ptr<IGraphicsPen> WxGraphicsRenderer::CreatePen(const Color& color, float width)
	{
		return std::make_shared<WxGraphicsPen>(*this, color, width);
	}
	std::shared_ptr<IGraphicsSolidBrush> WxGraphicsRenderer::CreateSolidBrush(const Color& color)
	{
		return std::make_shared<WxGraphicsSolidBrush>(*this, color);
	}
	std::shared_ptr<IGraphicsTextureBrush> WxGraphicsRenderer::CreateTextureBrush(const Image& image)
	{
		if (image)
		{
			return std::make_shared<WxGraphicsTextureBrush>(*this, image);
		}
		return {};
	}
	std::shared_ptr<IGraphicsTextureBrush> WxGraphicsRenderer::CreateTextureBrush(const GDIBitmap& bitmap)
	{
		if (bitmap)
		{
			return std::make_shared<WxGraphicsTextureBrush>(*this, bitmap);
		}
		return {};
	}
	std::shared_ptr<IGraphicsLinearGradientBrush> WxGraphicsRenderer::CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform)
	{
		return std::make_shared<WxGraphicsLinearGradientBrush>(*this, rect, colors, std::move(transform));
	}
	std::shared_ptr<IGraphicsRadialGradientBrush> WxGraphicsRenderer::CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform)
	{
		return std::make_shared<WxGraphicsRadialGradientBrush>(*this, rect, colors, std::move(transform));
	}

	// Path functions
	std::shared_ptr<IGraphicsPath> WxGraphicsRenderer::CreatePath()
	{
		return std::make_shared<WxGraphicsPath>(*this, m_Renderer.CreatePath());
	}

	// Texture functions
	std::shared_ptr<IGraphicsTexture> WxGraphicsRenderer::CreateTexture()
	{
		return std::make_shared<WxGraphicsTexture>(*this);
	}
	std::shared_ptr<IGraphicsTexture> WxGraphicsRenderer::CreateTexture(const Image& image)
	{
		if (image)
		{
			return std::make_shared<WxGraphicsTexture>(*this, image);
		}
		return nullptr;
	}
	std::shared_ptr<IGraphicsTexture> WxGraphicsRenderer::CreateTexture(const SizeF& size, const Color& color)
	{
		return std::make_shared<WxGraphicsTexture>(*this, size, color);
	}

	// Text functions
	std::shared_ptr<IGraphicsFont> WxGraphicsRenderer::CreateFont()
	{
		return std::make_shared<WxGraphicsFont>(*this);
	}
	std::shared_ptr<IGraphicsFont> WxGraphicsRenderer::CreateFont(const Font& font)
	{
		return std::make_shared<WxGraphicsFont>(*this, font);
	}
	std::shared_ptr<IGraphicsFont> WxGraphicsRenderer::CreateFont(float pointSize, const String& faceName)
	{
		return std::make_shared<WxGraphicsFont>(*this, pointSize, faceName);
	}
	std::shared_ptr<IGraphicsFont> WxGraphicsRenderer::CreateFont(const SizeF& pixelSize, const String& faceName)
	{
		return std::make_shared<WxGraphicsFont>(*this, pixelSize, faceName);
	}

	// WxGraphicsRenderer
	bool WxGraphicsRenderer::SupportBitmapRescaleOnDraw() const
	{
		return m_Type == Type::GDIPlus;
	}
}
