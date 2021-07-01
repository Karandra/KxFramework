#pragma once
#include "Common.h"
#include "GDIBrush.h"
#include "GDIPen.h"
#include "../GraphicsRenderer/IGraphicsRenderer.h"

namespace kxf
{
	class KX_API GDIGraphicsRenderer: public RTTI::ExtendInterface<GDIGraphicsRenderer, IGraphicsRenderer>
	{
		KxRTTI_DeclareIID(GDIGraphicsRenderer, {0x366d774d, 0x7570, 0x489d, {0xb5, 0xbc, 0x6, 0x8, 0xa6, 0xbc, 0x88, 0xa4}});

		friend class GDIGraphicsContext;

		private:
			mutable std::unique_ptr<IGraphicsPen> m_TransparentPen;
			mutable std::unique_ptr<IGraphicsBrush> m_TransparentBrush;

		protected:
			GDIPen ToGDIPen(const IGraphicsPen& pen) const;
			GDIBrush ToGDIBrush(const IGraphicsBrush& brush) const;

		public:
			GDIGraphicsRenderer() noexcept = default;

		public:
			// IGraphicsRenderer
			String GetName() const override;
			Version GetVersion() const override;

			std::unique_ptr<IGraphicsContext> CreateContext(std::shared_ptr<IGraphicsTexture> texture, wxWindow* window = nullptr) override;
			std::unique_ptr<IGraphicsContext> CreateGDIContext(wxDC& dc, const Size& size = Size::UnspecifiedSize()) override;
			std::unique_ptr<IGraphicsContext> CreateWindowContext(wxWindow& window) override;
			std::unique_ptr<IGraphicsContext> CreateWindowClientContext(wxWindow& window) override;
			std::unique_ptr<IGraphicsContext> CreateWindowPaintContext(wxWindow& window) override;
			std::unique_ptr<IGraphicsContext> CreateMeasuringContext(wxWindow* window = nullptr) override;

		public:
			// Pen and brush functions
			const IGraphicsPen& GetTransparentPen() const override;
			const IGraphicsBrush& GetTransparentBrush() const override;

			std::shared_ptr<IGraphicsPen> CreatePen(const Color& color, float width = 1.0f) override;
			std::shared_ptr<IGraphicsSolidBrush> CreateSolidBrush(const Color& color) override;
			std::shared_ptr<IGraphicsTextureBrush> CreateTextureBrush(const BitmapImage& image) override;
			std::shared_ptr<IGraphicsTextureBrush> CreateTextureBrush(const GDIBitmap& bitmap);
			std::shared_ptr<IGraphicsLinearGradientBrush> CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) override;
			std::shared_ptr<IGraphicsRadialGradientBrush> CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, const AffineMatrixF& transform = {}) override;

			// Path functions (not implemented)
			std::shared_ptr<IGraphicsPath> CreatePath() override
			{
				return nullptr;
			}

			// Texture functions
			std::shared_ptr<IGraphicsTexture> CreateTexture() override;
			std::shared_ptr<IGraphicsTexture> CreateTexture(const BitmapImage& image) override;
			std::shared_ptr<IGraphicsTexture> CreateTexture(const SVGImage& vectorImage) override;
			std::shared_ptr<IGraphicsTexture> CreateTexture(const GDIBitmap& bitmap);
			std::shared_ptr<IGraphicsTexture> CreateTexture(const SizeF& size, const Color& color) override;

			// Text functions
			std::shared_ptr<IGraphicsFont> CreateFont() override;
			std::shared_ptr<IGraphicsFont> CreateFont(const Font& font) override;
			std::shared_ptr<IGraphicsFont> CreateFont(float pointSize, const String& faceName) override;
			std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName) override;
	};
}
