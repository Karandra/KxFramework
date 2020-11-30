#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"
class wxWindow;
class wxDC;

namespace kxf
{
	class Font;
	class Image;

	class IGraphicsContext;
	class IGraphicsTexture;
	class IGraphicsPath;
	class IGraphicsFont;
	class IGraphicsPen;
	class IGraphicsBrush;
	class IGraphicsSolidBrush;
	class IGraphicsTextureBrush;
	class IGraphicsLinearGradientBrush;
	class IGraphicsRadialGradientBrush;
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
			virtual std::unique_ptr<IGraphicsContext> CreateGDIContext(wxDC& dc) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateWindowContext(wxWindow& window) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateWindowClientContext(wxWindow& window) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateWindowPaintContext(wxWindow& window) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateMeasuringContext() = 0;

		public:
			// Pen and brush functions
			virtual const IGraphicsPen& GetTransparentPen() const = 0;
			virtual const IGraphicsBrush& GetTransparentBrush() const = 0;

			virtual std::shared_ptr<IGraphicsPen> CreatePen(const Color& color, float width = 1.0f) = 0;
			virtual std::shared_ptr<IGraphicsSolidBrush> CreateSolidBrush(const Color& color) = 0;
			virtual std::shared_ptr<IGraphicsTextureBrush> CreateTextureBrush(const Image& image) = 0;
			virtual std::shared_ptr<IGraphicsLinearGradientBrush> CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) = 0;
			virtual std::shared_ptr<IGraphicsRadialGradientBrush> CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) = 0;

			// Path functions
			virtual std::shared_ptr<IGraphicsPath> CreatePath() = 0;

			// Texture functions
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture() = 0;
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture(const Image& image) = 0;
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture(const SizeF& size, const Color& color) = 0;

			// Text functions
			virtual std::shared_ptr<IGraphicsFont> CreateFont() = 0;
			virtual std::shared_ptr<IGraphicsFont> CreateFont(const Font& font) = 0;
			virtual std::shared_ptr<IGraphicsFont> CreateFont(float pointSize, const String& faceName) = 0;
			virtual std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName) = 0;
	};
}
