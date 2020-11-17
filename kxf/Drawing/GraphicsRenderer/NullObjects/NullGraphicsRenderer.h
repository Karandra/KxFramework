#pragma once
#include "../Common.h"
#include "../IGraphicsRenderer.h"
#include "../IGraphicsContext.h"
#include "../IGraphicsTexture.h"
#include "../IGraphicsPen.h"
#include "../IGraphicsBrush.h"
#include "../IGraphicsPath.h"
#include "../IGraphicsFont.h"

namespace kxf::Drawing
{
	class KX_API NullGraphicsRenderer final: public IGraphicsRenderer
	{
		public:
			// IGraphicsRenderer
			String GetName() const override
			{
				return {};
			}
			Version GetVersion() const override
			{
				return {};
			}

			std::unique_ptr<IGraphicsContext> CreateContext(std::shared_ptr<IGraphicsTexture> texture) override
			{
				return nullptr;
			}
			std::unique_ptr<IGraphicsContext> CreateGDIContext(wxDC& dc) override
			{
				return nullptr;
			}
			std::unique_ptr<IGraphicsContext> CreateWindowContext(wxWindow& window) override
			{
				return nullptr;
			}
			std::unique_ptr<IGraphicsContext> CreateWindowClientContext(wxWindow& window) override
			{
				return nullptr;
			}
			std::unique_ptr<IGraphicsContext> CreateWindowPaintContext(wxWindow& window) override
			{
				return nullptr;
			}
			std::unique_ptr<IGraphicsContext> CreateMeasuringContext() override
			{
				return nullptr;
			}

		public:
			// Pen and brush functions
			std::shared_ptr<IGraphicsPen> CreatePen(const Color& color, float width = 1.0f) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsSolidBrush> CreateSolidBrush(const Color& color) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsTextureBrush> CreateTextureBrush(const Image& image) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsLinearGradientBrush> CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsRadialGradientBrush> CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) override
			{
				return nullptr;
			}

			// Path functions
			std::shared_ptr<IGraphicsPath> CreatePath() override
			{
				return nullptr;
			}

			// Texture functions
			std::shared_ptr<IGraphicsTexture> CreateTexture() override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsTexture> CreateTexture(const Image& image) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsTexture> CreateTexture(const SizeF& size, const Color& color) override
			{
				return nullptr;
			}

			// Text functions
			std::shared_ptr<IGraphicsFont> CreateFont() override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsFont> CreateFont(const GDIFont& font) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsFont> CreateFont(float pointSize, const String& faceName) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName) override
			{
				return nullptr;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsRenderer NullGraphicsRenderer;
}
