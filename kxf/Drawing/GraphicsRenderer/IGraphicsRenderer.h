#pragma once
#include "Common.h"
#include "IGraphicsContext.h"
#include "IGraphicsTexture.h"
#include "IGraphicsMatrix.h"
#include "IGraphicsPen.h"
#include "IGraphicsBrush.h"
#include "IGraphicsPath.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"
class wxWindow;

namespace kxf
{
	class Image;
	class GDIFont;
}

namespace kxf
{
	class KX_API IGraphicsRenderer: public RTTI::Interface<IGraphicsRenderer>
	{
		KxRTTI_DeclareIID(IGraphicsRenderer, {0xf8abb3f6, 0xee8d, 0x49d0, {0x96, 0xe5, 0x2f, 0xd6, 0xaa, 0x3c, 0x5d, 0x6f}});

		public:
			virtual ~IGraphicsRenderer() = default;

		public:
			virtual String GetName() const = 0;
			virtual Version GetVersion() const = 0;

			virtual std::unique_ptr<IGraphicsContext> CreateContext(std::shared_ptr<IGraphicsTexture> texture) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateWindowContext(wxWindow& window) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateWindowClientContext(wxWindow& window) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateWindowPaintContext(wxWindow& window) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateMeasuringContext() = 0;

		public:
			// Transformation matrix
			virtual std::shared_ptr<IGraphicsMatrix> CreateMatrix(float m11, float m12, float m21, float m22, float tx, float ty) = 0;
			std::shared_ptr<IGraphicsMatrix> CreateIdentityMatrix(float m11, float m12, float m21, float m22, float tx, float ty)
			{
				return CreateMatrix(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
			}

			// Pen and brush functions
			virtual std::shared_ptr<IGraphicsPen> CreatePen(const Color& color, float width = 1.0f) = 0;
			virtual std::shared_ptr<IGraphicsSolidBrush> CreateSolidBrush(const Color& color) = 0;
			virtual std::shared_ptr<IGraphicsTextureBrush> CreateTextureBrush(const Image& image) = 0;
			virtual std::shared_ptr<IGraphicsLinearGradientBrush> CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> transform = {}) = 0;
			virtual std::shared_ptr<IGraphicsRadialGradientBrush> CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> transform = {}) = 0;

			// Path functions
			virtual std::shared_ptr<IGraphicsPath> CreatePath() = 0;

			// Texture functions
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture() = 0;
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture(const Image& image) = 0;
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture(const SizeF& size, const Color& color) = 0;

			// Text functions
			virtual std::shared_ptr<IGraphicsFont> CreateFont() = 0;
			virtual std::shared_ptr<IGraphicsFont> CreateFont(const GDIFont& font, const Color& color = Drawing::GetStockColor(StockColor::Black)) = 0;
			virtual std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName, const Color& color = Drawing::GetStockColor(StockColor::Black)) = 0;

	};
}

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
			// Transformation matrix
			std::shared_ptr<IGraphicsMatrix> CreateMatrix(float m11, float m12, float m21, float m22, float tx, float ty) override
			{
				return nullptr;
			}

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
			std::shared_ptr<IGraphicsLinearGradientBrush> CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> transform = {}) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsRadialGradientBrush> CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> transform = {}) override
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
			std::shared_ptr<IGraphicsFont> CreateFont(const GDIFont& font, const Color& color = Drawing::GetStockColor(StockColor::Black)) override
			{
				return nullptr;
			}
			std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName, const Color& color = Drawing::GetStockColor(StockColor::Black)) override
			{
				return nullptr;
			}
	};
}
namespace kxf
{
	inline const Drawing::NullGraphicsRenderer NullGraphicsRenderer;
}
