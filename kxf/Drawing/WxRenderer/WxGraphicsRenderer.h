#pragma once
#include "Common.h"
#include "../GraphicsRenderer/IGraphicsRenderer.h"
#include "../GDIRenderer/GDIBitmap.h"
class wxGraphicsRenderer;

namespace kxf
{
	class KX_API WxGraphicsRenderer: public RTTI::ExtendInterface<WxGraphicsRenderer, IGraphicsRenderer>
	{
		public:
			enum class Type
			{
				None = -1,

				GDIPlus,
				Direct2D,
				Cairo
			};

		private:
			wxGraphicsRenderer& m_Renderer;
			Type m_Type = Type::None;

			mutable GDIBitmap m_TransparenBitmap;
			mutable std::shared_ptr<IGraphicsPen> m_TransparentPen;
			mutable std::shared_ptr<IGraphicsBrush> m_TransparentBrush;

		public:
			WxGraphicsRenderer(wxGraphicsRenderer& renderer);

		public:
			// IGraphicsRenderer
			String GetName() const override;
			Version GetVersion() const override;

			std::unique_ptr<IGraphicsContext> CreateContext(std::shared_ptr<IGraphicsTexture> texture) override;
			std::unique_ptr<IGraphicsContext> CreateGDIContext(wxDC& dc) override;
			std::unique_ptr<IGraphicsContext> CreateWindowContext(wxWindow& window) override;
			std::unique_ptr<IGraphicsContext> CreateWindowClientContext(wxWindow& window) override;
			std::unique_ptr<IGraphicsContext> CreateWindowPaintContext(wxWindow& window) override;
			std::unique_ptr<IGraphicsContext> CreateMeasuringContext() override;

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

			// Path functions
			std::shared_ptr<IGraphicsPath> CreatePath() override;

			// Texture functions
			std::shared_ptr<IGraphicsTexture> CreateTexture() override;
			std::shared_ptr<IGraphicsTexture> CreateTexture(const BitmapImage& image) override;
			std::shared_ptr<IGraphicsTexture> CreateTexture(const SizeF& size, const Color& color) override;

			// Text functions
			std::shared_ptr<IGraphicsFont> CreateFont() override;
			std::shared_ptr<IGraphicsFont> CreateFont(const Font& font) override;
			std::shared_ptr<IGraphicsFont> CreateFont(float pointSize, const String& faceName) override;
			std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName) override;

			// WxGraphicsRenderer
			const wxGraphicsRenderer& Get() const noexcept
			{
				return m_Renderer;
			}
			wxGraphicsRenderer& Get() noexcept
			{
				return m_Renderer;
			}

			Type GetType() const
			{
				return m_Type;
			}
			bool CanRescaleBitmapOnDraw() const;
			bool CanDrawNullBitmap() const;

			const GDIBitmap& GetTransparentBitmap() const;

			AffineMatrixF ToAffineMatrix(const wxGraphicsMatrix& matrix)
			{
				wxDouble m11 = 0;
				wxDouble m12 = 0;
				wxDouble m21 = 0;
				wxDouble m22 = 0;
				wxDouble tx = 0;
				wxDouble ty = 0;
				matrix.Get(&m11, &m12, &m21, &m22, &tx, &ty);

				return {m11, m12, m21, m22, tx, ty};
			}
			wxGraphicsMatrix FromAffineMatrix(const AffineMatrixF& matrix) const
			{
				wxDouble m11 = 0;
				wxDouble m12 = 0;
				wxDouble m21 = 0;
				wxDouble m22 = 0;
				wxDouble tx = 0;
				wxDouble ty = 0;
				matrix.GetElements(m11, m12, m21, m22, tx, ty);

				return m_Renderer.CreateMatrix(m11, m12, m21, m22, tx, ty);
			}
	};
}
