#pragma once
#include "Common.h"
#include "IGraphicsObject.h"
#include "../GradientStops.h"
class wxWindow;

namespace kxf
{
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
	class IGraphicsSolidBrush;
	class IGraphicsTextureBrush;
	class IGraphicsLinearGradientBrush;
	class IGraphicsRadialGradientBrush;
}

namespace kxf
{
	enum class GraphicsContextFeature: uint32_t
	{
		None = 0,

		ClippingRegion = 1 << 0,
		TransformationMatrix = 1 << 1,
		DrawPath = 1 << 2,
		DrawText = 1 << 3,
		DrawShape = 1 << 4,
		DrawTexture = 1 << 5,
		TextMeasurement = 1 << 6,
		Antialiasing = 1 << 7,
		Interpolation = 1 << 8,
		Layers = 1 << 9,
		States = 1 << 10
	};
}

namespace kxf
{
	class KX_API IGraphicsContext: public RTTI::ExtendInterface<IGraphicsContext, IGraphicsObject>
	{
		KxRTTI_DeclareIID(IGraphicsContext, {0x7a91357c, 0x87f9, 0x4230, {0x92, 0xf, 0x54, 0xbf, 0xde, 0x42, 0x35, 0xf5}});

		public:
			virtual ~IGraphicsContext() = default;

		public:
			// Feature support
			virtual FlagSet<GraphicsContextFeature> GetSupportedFeatures() const = 0;

			// Clipping region functions
			virtual void ClipRegion(const Region& region) = 0;
			virtual void ClipBoxRegion(const RectF& rect) = 0;
			virtual void ResetClipRegion() = 0;
			virtual RectF GetClipBox() const = 0;

			// Transformation matrix
			virtual std::shared_ptr<IGraphicsMatrix> GetTransform() const = 0;
			virtual bool SetTransform(std::shared_ptr<IGraphicsMatrix> transform) = 0;

			virtual bool TransformInvert() = 0;
			virtual void TransformRotate(Angle angle) = 0;
			virtual void TransformScale(float xScale, float yScale) = 0;
			virtual void TransformTranslate(const Size& dxy) = 0;
			virtual void TransformConcat(const IGraphicsMatrix& matrix) = 0;

			// Pen and brush functions
			virtual std::shared_ptr<IGraphicsPen> GetPen() const = 0;
			virtual void SetPen(std::shared_ptr<IGraphicsPen> pen) = 0;

			virtual std::shared_ptr<IGraphicsBrush> GetBrush() const = 0;
			virtual void SetBrush(std::shared_ptr<IGraphicsBrush> brush) = 0;

			// Path functions
			virtual void StrokePath(const IGraphicsPath& path) = 0;
			virtual void FillPath(const IGraphicsPath& path, PolygonFill fill = PolygonFill::OddEvenRule) = 0;
			virtual void DrawPath(const IGraphicsPath& path, PolygonFill fill = PolygonFill::OddEvenRule) = 0;

			// Texture functions
			virtual void DrawTexture(const IGraphicsTexture& texture, const RectF& rect) = 0;
			virtual void DrawTexture(const Image& image, const RectF& rect) = 0;

			// Text functions
			virtual std::shared_ptr<IGraphicsFont> GetFont() const = 0;
			virtual void SetFont(std::shared_ptr<IGraphicsFont> font) = 0;

			virtual GraphicsTextExtent GetTextExtent(const String& text) const = 0;
			virtual std::vector<float> GetPartialTextExtent(const String& text) const = 0;

			virtual void DrawText(const String& text, const PointF& point) = 0;
			virtual void DrawText(const String& text, const PointF& point, const IGraphicsBrush& brush) = 0;

			virtual void DrawRotatedText(const String& text, const PointF& point, Angle angle) = 0;
			virtual void DrawRotatedText(const String& text, const PointF& point, Angle angle, const IGraphicsBrush& brush) = 0;

			virtual RectF DrawLabel(const String& text, const RectF& rect, const IGraphicsTexture& icon, FlagSet<Alignment> alignment = Alignment::Left|Alignment::Top, size_t acceleratorIndex = String::npos) = 0;
			virtual RectF DrawLabel(const String& text, const RectF& rect, FlagSet<Alignment> alignment = Alignment::Left|Alignment::Top, size_t acceleratorIndex = String::npos) = 0;

			// Drawing functions
			virtual void Clear(const IGraphicsBrush& brush) = 0;

			virtual void DrawCircle(const Point& pos, float radius) = 0;

			virtual void DrawEllipse(const RectF& rect) = 0;
			void DrawEllipse(const Point& pos, const Size& size)
			{
				DrawEllipse({pos, size});
			}

			virtual void DrawRectangle(const RectF& rect) = 0;
			void DrawRectangle(const Point& pos, const Size& size)
			{
				DrawRectangle({pos, size});
			}

			virtual void DrawRoundedRectangle(const RectF& rect, float radius) = 0;
			void DrawRoundedRectangle(const Point& pos, const Size& size, float radius)
			{
				DrawRoundedRectangle({pos, size}, radius);
			}

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

			virtual CompositionMode GetCompositionMode() const = 0;
			virtual void SetCompositionMode(CompositionMode mode) = 0;

			virtual InterpolationQuality GetInterpolationQuality() const = 0;
			virtual void SetInterpolationQuality(InterpolationQuality quality) = 0;

			// Bounding box functions
			virtual RectF GetBoundingBox() const = 0;
			virtual void CalcBoundingBox(const PointF& point) = 0;
			virtual void ResetBoundingBox() = 0;

			// Page and document start/end functions
			virtual bool StartDocument(const String& message) = 0;
			virtual void EndDocument() = 0;

			virtual void StartPage() = 0;
			virtual void EndPage() = 0;

			// Modifying the state
			virtual void BeginLayer(float opacity = 1.0f) = 0;
			virtual void EndLayer() = 0;

			virtual void PushState() = 0;
			virtual void PopState() = 0;

			virtual void Flush() = 0;

			// Offset management
			virtual bool ShouldOffset() const = 0;
			virtual void EnableOffset(bool enable = true) = 0;
	};
}
