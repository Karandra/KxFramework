#pragma once
#include "Common.h"
#include "IGraphicsObject.h"

namespace kxf
{
	class wxWindow;

	class Region;
	class Bitmap;
	class Image;
	class Font;
	class String;

	class IGraphicsRenderer;
	class IGraphicsTexture;
	class IGraphicsMatrix;
	class IGraphicsPath;
	class IGraphicsFont;
	class IGraphicsPen;
	class IGraphicsBrush;
	class IGraphicsLinearGradientBrush;
	class IGraphicsRadialGradientBrush;
}

namespace kxf
{
	class KX_API IGraphicsContext: public RTTI::ExtendInterface<IGraphicsContext, IGraphicsObject>
	{
		KxRTTI_DeclareIID(IGraphicsContext, {0x7a91357c, 0x87f9, 0x4230, {0x92, 0xf, 0x54, 0xbf, 0xde, 0x42, 0x35, 0xf5}});

		public:
			virtual ~IGraphicsContext() = default;

		public:
			// Clipping region functions
			virtual void SetClippingRegion(const Region& region) = 0;
			virtual void SetClippingRegion(const RectF& rect) = 0;
			virtual void ResetClippingRegion() = 0;
			virtual RectF GetClippingBox() const = 0;

			// Transformation matrix
			virtual std::shared_ptr<IGraphicsMatrix> CreateMatrix(float m11, float m12, float m21, float m22, float tx, float ty) const = 0;
			std::shared_ptr<IGraphicsMatrix> CreateIdentityMatrix(float m11, float m12, float m21, float m22, float tx, float ty)
			{
				return CreateMatrix(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
			}

			virtual std::shared_ptr<IGraphicsMatrix> GetTransform() const = 0;
			virtual void SetTransform(std::shared_ptr<IGraphicsMatrix> matrix) = 0;

			virtual void Rotate(Angle angle) = 0;
			virtual void Scale(float xScale, float yScale) = 0;
			virtual void Translate(const Size& dxy) = 0;

			// Pen and brush functions
			virtual std::shared_ptr<IGraphicsPen> CreatePen() = 0;
			virtual std::shared_ptr<IGraphicsBrush> CreateBrush() = 0;
			virtual std::shared_ptr<IGraphicsLinearGradientBrush> CreateLinearGradientBrush(const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> matrix = {}) = 0;
			virtual std::shared_ptr<IGraphicsRadialGradientBrush> CreateRadialGradientBrush(const RectF& rect, const GradientStops& colors, std::shared_ptr<IGraphicsMatrix> matrix = {}) = 0;

			virtual void SetPen(std::shared_ptr<IGraphicsBrush> brush) = 0;
			virtual void SetBrush(std::shared_ptr<IGraphicsPen> pen) = 0;

			// Path functions
			virtual std::shared_ptr<IGraphicsPath> CreatePath() = 0;

			virtual void StrokePath(const IGraphicsPath& path) = 0;
			virtual void FillPath(const IGraphicsPath& path, PolygonFill fill = PolygonFill::OddEvenRule) = 0;
			virtual void DrawPath(const IGraphicsPath& path, PolygonFill fill = PolygonFill::OddEvenRule) = 0;

			// Texture functions
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture() = 0;
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture(const Image& image) = 0;
			virtual std::shared_ptr<IGraphicsTexture> CreateTexture(const Bitmap& bitmap) = 0;

			// Text functions
			virtual std::shared_ptr<IGraphicsFont> CreateFont() = 0;
			virtual std::shared_ptr<IGraphicsFont> CreateFont(const Font& font, const Color& color = Drawing::GetStockColor(StockColor::Black)) = 0;
			virtual std::shared_ptr<IGraphicsFont> CreateFont(const SizeF& pixelSize, const String& faceName, const Color& color = Drawing::GetStockColor(StockColor::Black)) = 0;

			virtual std::shared_ptr<IGraphicsFont> GetFont() const = 0;
			virtual void SetFont(std::shared_ptr<IGraphicsFont> font) = 0;

			virtual GraphicsTextExtent GetTextExtent(const String& text) const = 0;
			virtual std::vector<float> GetPartialTextExtent(const String& text) const = 0;

			virtual void DrawText(const String& text, const PointF& point) = 0;
			virtual void DrawText(const String& text, const PointF& point, Angle angle) = 0;
			virtual void DrawText(const String& text, const PointF& point, const IGraphicsBrush& backgroundBrush) = 0;
			virtual void DrawText(const String& text, const PointF& point, Angle angle, const IGraphicsBrush& backgroundBrush) = 0;

			// Drawing functions
			virtual void DrawTexture(const IGraphicsTexture& texture, const RectF& rect) = 0;
			virtual void DrawTexture(const Bitmap& bitmap, const RectF& rect) = 0;
			virtual void DrawTexture(const Image& image, const RectF& rect) = 0;

			virtual void DrawEllipse(const RectF& rect) = 0;
			virtual void DrawRect(const RectF& rect) = 0;
			virtual void DrawRoundedRect(const RectF& rect, float radius) = 0;
			virtual void DrawLine(const PointF& point1, const PointF& point2) = 0;

			virtual void DrawPolyLine(const PointF* points, size_t count) = 0;

			template<size_t N>
			void DrawPolyLine(const PointF(&points)[N])
			{
				DrawPolyLine(points, N);
			}

			template<size_t N>
			void DrawPolyLine(const std::array<PointF, N>& points)
			{
				DrawPolyLine(points.data(), points.size());
			}

			virtual void DrawDisconnectedLines(const PointF* startPoints, const PointF* endPoints, size_t count) = 0;

			template<size_t N>
			void DrawDisconnectedLines(const PointF(&startPoints)[N], const PointF(&endPoints)[N])
			{
				DrawDisconnectedLines(startPoints, endPoints, N);
			}

			template<size_t N>
			void DrawDisconnectedLines(const std::array<PointF, N>& startPoints, const std::array<PointF, N>& endPoints)
			{
				DrawDisconnectedLines(startPoints.data(), endPoints.data(), N);
			}

			// Getting and setting parameters
			virtual SizeF GetSize() const = 0;
			virtual SizeF GetPPI() const = 0;
			virtual wxWindow* GetWindow() const = 0;

			virtual AntialiasMode GetAntialiasMode() const = 0;
			virtual void SetAntialiasMode(AntialiasMode mode) = 0;

			virtual CompositingMode GetCompositingMode() const = 0;
			virtual void SetCompositingMode(CompositingMode mode) = 0;

			virtual InterpolationMode GetInterpolationMode() const = 0;
			virtual void SetInterpolationMode(InterpolationMode mode) = 0;

			// Page and document start/end functions
			virtual void StartDocument(const String& message) = 0;
			virtual void EndDocument() = 0;

			virtual void StartPage() = 0;
			virtual void EndPage() = 0;

			// Modifying the state
			virtual void BeginLayer(float opacity = 1.0f) = 0;
			virtual void EndLayer() = 0;

			virtual void PushState() = 0;
			virtual void PopState() = 0;

			// Offset management
			virtual bool ShouldOffset() const = 0;
			virtual void EnableOffset(bool enable = true) = 0;
	};
}
