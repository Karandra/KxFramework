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
		switch (m_Type)
		{
			case Type::GDIPlus:
			{
				return wxS("GDIPlus");
			}
			case Type::Direct2D:
			{
				return wxS("Direct2D");
			}
			case Type::Cairo:
			{
				return wxS("Cairo");
			}
		};
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
		// Context created for 'wxBufferedPaintDC' doesn't work correctly with Direct2D and
		// leaves black surface after flushing, so we're going to use regular 'wxPaintDC' here.
		// It's still buffered by the Direct2D anyway.

		if (m_Type == Type::Direct2D || window.IsDoubleBuffered())
		{
			return std::make_unique<WxGraphicsPaintContext>(*this, window);
		}
		else
		{
			return std::make_unique<WxGraphicsBufferedPaintContext>(*this, window);
		}
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
	bool WxGraphicsRenderer::CanRescaleBitmapOnDraw() const
	{
		// GDIPlus can do stretch-scale but it doesn't respect the interpolation quality option
		// or interprets it incorrectly. It works fast when it's set to best quality and slow if
		// set to fast. Guess now it's best to let it do the rescaling.

		// Some versions of Direct2D can only crop the image if it's larger than the provided rectangle,
		// other versions can actually rescale texture and rescale it fast. Cropping was observed on Windows 7,
		// but on Windows 10 (v20H2 at least) it does proper fast rescaling.

		// No information for Cairo, assuming it can't rescale. Testing required.

		return m_Type == Type::GDIPlus || m_Type == Type::Direct2D;
	}
	bool WxGraphicsRenderer::CanDrawNullBitmap() const
	{
		// There is a bug where Direct2D renderer crashes when it asked to draw a null bitmap.
		// It doesn't check anything and faces a nullptr somewhere deep inside its wx-side implementation.
		return m_Type != Type::Direct2D;
	}

	const GDIBitmap& WxGraphicsRenderer::GetTransparentBitmap() const
	{
		if (!m_TransparenBitmap)
		{
			m_TransparenBitmap = GDIBitmap({8, 8}, ColorDepthDB::BPP32);
		}
		return m_TransparenBitmap;
	}
	const IGraphicsPen& WxGraphicsRenderer::GetTransparentPen() const
	{
		if (!m_TransparentPen)
		{
			m_TransparentPen = const_cast<WxGraphicsRenderer&>(*this).CreatePen(Drawing::GetStockColor(StockColor::Transparent), 1.0f);
		}
		return *m_TransparentPen;
	}
	const IGraphicsBrush& WxGraphicsRenderer::GetTransparentBrush() const
	{
		if (!m_TransparentBrush)
		{
			m_TransparentBrush = const_cast<WxGraphicsRenderer&>(*this).CreateSolidBrush(Drawing::GetStockColor(StockColor::Transparent));
		}
		return *m_TransparentBrush;
	}
}
