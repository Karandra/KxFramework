#pragma once
#include "Common.h"
#include "Color.h"
#include "Icon.h"
#include "Bitmap.h"
#include "Region.h"
#include "Font.h"
#include "Brush.h"
#include "Pen.h"
#include "ColorDepth.h"
#include "IGDIObject.h"
#include "kxf/UI/Common.h"
#include <wx/dc.h>
#include <wx/graphics.h>

namespace kxf
{
	enum class GDIMappingMode
	{
		Text = wxMM_TEXT,
		Metric = wxMM_METRIC,
		LoMetric = wxMM_LOMETRIC,
		Twips = wxMM_TWIPS,
		Points = wxMM_POINTS,
	};
	enum class GDILogicalFunction
	{
		Nop = wxNO_OP, // dst

		Set = wxSET, // 1
		Clear = wxCLEAR, // 0
		Invert = wxINVERT, // NOT dst
		Copy = wxCOPY, // src
		And = wxAND, // src AND dst
		Or = wxOR, // src OR dst
		Nor = wxNOR, // (NOT src) AND (NOT dst)
		Nand = wxNAND, // (NOT src) OR (NOT dst)
		Equiv = wxEQUIV, // (NOT src) XOR dst

		OrReverse = wxOR_REVERSE, // src OR (NOT dst)
		AndReverse = wxAND_REVERSE, // src AND (NOT dst)
		AndInvert = wxAND_INVERT, // (NOT src) AND dst
		OrInvert = wxOR_INVERT, // (NOT src) OR dst
		SrcInvert = wxSRC_INVERT, // (NOT src)
	};

	class TextExtent final
	{
		private:
			Size m_Extent;
			FontMetrics m_FontMetrics;

		public:
			constexpr TextExtent(Size extent, FontMetrics metrics = {}) noexcept
				:m_Extent(std::move(extent)), m_FontMetrics(std::move(metrics))
			{
			}

		public:
			constexpr Size GetExtent() const noexcept
			{
				return m_Extent;
			}
			constexpr FontMetrics GetFontMetrics() const noexcept
			{
				return m_FontMetrics;
			}
	};
}

namespace kxf
{
	class KX_API GDICanvas: public RTTI::ExtendInterface<GDICanvas, IGDIObject>
	{
		KxRTTI_DeclareIID(GDICanvas, {0x7f0f1843, 0x27b9, 0x40df, {0x8d, 0xdc, 0x27, 0x98, 0x52, 0x44, 0xb7, 0x20}});

		protected:
			template<class T>
			using TCoordPair = typename Geometry::OrderedPairTemplate<T, int>;

		private:
			wxDC* m_DC = nullptr;

		public:
			GDICanvas() = default;
			GDICanvas(wxDC& other)
				:m_DC(&other)
			{
			}
			GDICanvas(const GDICanvas&) = delete;
			virtual ~GDICanvas() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_DC || !m_DC->IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<GDICanvas>())
				{
					return m_DC == object->m_DC || ((m_DC && object->m_DC) && m_DC->IsSameAs(*object->m_DC));
				}
				return false;
			}
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return nullptr;
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// GDICanvas
			const wxDC& ToWxDC() const noexcept
			{
				return *m_DC;
			}
			wxDC& ToWxDC() noexcept
			{
				return *m_DC;
			}

			void CopyAttributes(const GDICanvas& other)
			{
				m_DC->CopyAttributes(*other.m_DC);
			}
			Color GetPixel(const Point& point) const
			{
				wxColour color;
				if (m_DC->GetPixel(point, &color))
				{
					return color;
				}
				return {};
			}
			Bitmap ToBitmap() const
			{
				return m_DC->GetAsBitmap();
			}
			Bitmap ToBitmap(const Rect& subRect) const
			{
				wxRect subRectWx = subRect;
				return m_DC->GetAsBitmap(&subRectWx);
			}

			ColorDepth GetDepth() const
			{
				return m_DC->GetDepth();
			}
			Size GetSize() const
			{
				return m_DC->GetSize();
			}
			int GetWidth() const
			{
				return m_DC->GetSize().GetWidth();
			}
			int GetHeight() const
			{
				return m_DC->GetSize().GetHeight();
			}

			Size GetSizeMM() const
			{
				return m_DC->GetSizeMM();
			}
			Size GetPPI() const
			{
				return m_DC->GetPPI();
			}

			SizeD GetUserScale() const
			{
				double x = 0;
				double y = 0;
				m_DC->GetUserScale(&x, &y);
				return {};
			}
			void SetUserScale(const SizeD& userScale)
			{
				m_DC->SetUserScale(userScale.GetX(), userScale.GetY());
			}

			SizeD GetLogicalScale() const
			{
				double x = 0;
				double y = 0;
				m_DC->GetLogicalScale(&x, &y);
				return {};
			}
			void SetLogicalScale(const SizeD& userScale)
			{
				m_DC->SetLogicalScale(userScale.GetX(), userScale.GetY());
			}

			Point GetLogicalOrigin() const
			{
				return m_DC->GetLogicalOrigin();
			}
			void SetLogicalOrigin(const Point& point)
			{
				m_DC->SetLogicalOrigin(point.GetX(), point.GetY());
			}

			Point GetDeviceOrigin() const
			{
				return m_DC->GetDeviceOrigin();
			}
			void SetDeviceOrigin(const Point& point)
			{
				m_DC->SetDeviceOrigin(point.GetX(), point.GetY());
			}
			void SetDeviceLocalOrigin(const Point& point)
			{
				m_DC->SetDeviceLocalOrigin(point.GetX(), point.GetY());
			}
			void SetAxisOrientation(bool xLeftRight, bool yBottomUp)
			{
				m_DC->SetAxisOrientation(xLeftRight, yBottomUp);
			}

			GDIMappingMode GetMapMode() const
			{
				return static_cast<GDIMappingMode>(m_DC->GetMapMode());
			}
			void SetMapMode(GDIMappingMode mode) const
			{
				m_DC->SetMapMode(static_cast<wxMappingMode>(mode));
			}

			GDILogicalFunction GetLogicalFunction() const
			{
				return static_cast<GDILogicalFunction>(m_DC->GetLogicalFunction());
			}
			void SetLogicalFunction(GDILogicalFunction mode) const
			{
				m_DC->SetLogicalFunction(static_cast<wxRasterOperationMode>(mode));
			}

			// Graphics Context
			std::unique_ptr<wxGraphicsContext> GetGraphicsContext() const
			{
				return std::unique_ptr<wxGraphicsContext>(m_DC->GetGraphicsContext());
			}
			void SetGraphicsContext(std::unique_ptr<wxGraphicsContext> gc)
			{
				m_DC->SetGraphicsContext(gc.release());
			}

			// Coordinate conversion functions
			template<class TDerived, class TPair = TDerived>
			TPair DeviceToLogical(const TCoordPair<TDerived>& pair) const
			{
				return {m_DC->DeviceToLogicalX(pair.GetX()), m_DC->DeviceToLogicalY(pair.GetY())};
			}

			template<class TDerived, class TPair = TDerived>
			TPair DeviceToLogicalRelative(const TCoordPair<TDerived>& pair) const
			{
				return {m_DC->DeviceToLogicalXRel(pair.GetX()), m_DC->DeviceToLogicalYRel(pair.GetY())};
			}

			Rect DeviceToLogical(const Rect& rect) const
			{
				return {DeviceToLogical(rect.GetPosition()), DeviceToLogicalRelative(rect.GetSize())};
			}

			template<class TDerived, class TPair = TDerived>
			TPair LogicalToDevice(const TCoordPair<TDerived>& pair) const
			{
				return {m_DC->LogicalToDeviceX(pair.GetX()), m_DC->LogicalToDeviceY(pair.GetY())};
			}

			template<class TDerived, class TPair = TDerived>
			TPair LogicalToDeviceRelative(const TCoordPair<TDerived>& pair) const
			{
				return {m_DC->LogicalToDeviceXRel(pair.GetX()), m_DC->LogicalToDeviceYRel(pair.GetY())};
			}

			Rect LogicalToDevice(const Rect& rect) const
			{
				return {LogicalToDevice(rect.GetPosition()), LogicalToDeviceRelative(rect.GetSize())};
			}

			// Drawing functions
			void Clear()
			{
				m_DC->Clear();
			}
			void DrawArc(const Point& start, const Point& end, const Point& center)
			{
				m_DC->DrawArc(start, end, center);
			}
			void DrawCircle(const Point& pos, int radius)
			{
				m_DC->DrawCircle(pos, radius);
			}
			void DrawEllipse(const Rect& rect)
			{
				m_DC->DrawEllipse(rect);
			}
			void DrawEllipse(const Point& pos, const Size& size)
			{
				m_DC->DrawEllipse(pos, size);
			}
			void DrawEllipticArc(const Rect& rect, Angle start, Angle end)
			{
				m_DC->DrawEllipticArc(rect.GetPosition(), rect.GetSize(), start.ToDegrees(), end.ToDegrees());
			}
			void DrawEllipticArc(const Point& pos, const Size& size, Angle start, Angle end)
			{
				m_DC->DrawEllipticArc(pos, size, start.ToDegrees(), end.ToDegrees());
			}
			void DrawLine(const Point& pos1, const Point& pos2)
			{
				m_DC->DrawLine(pos1, pos2);
			}

			void DrawPolyLine(const Point* points, size_t count, const Point& offset = {0, 0})
			{
				std::vector<wxPoint> pointsBuffer = {points, points + count};
				m_DC->DrawLines(static_cast<int>(pointsBuffer.size()), pointsBuffer.data(), offset.GetX(), offset.GetY());
			}

			template<size_t N>
			void DrawPolyLine(const Point(&points)[N], const Point& offset = {0, 0})
			{
				std::array<wxPoint, N> pointsBuffer;
				std::copy_n(std::begin(points), N, pointsBuffer.begin());

				m_DC->DrawLines(static_cast<int>(pointsBuffer.size()), offset.GetX(), offset.GetY());
			}

			template<size_t N>
			void DrawPolyLine(const std::array<Point, N>& points, const Point& offset = {0, 0})
			{
				std::array<wxPoint, N> pointsBuffer;
				std::copy_n(std::begin(points), N, pointsBuffer.begin());

				m_DC->DrawLines(static_cast<int>(pointsBuffer.size()), offset.GetX(), offset.GetY());
			}

			void DrawSpline(const Point* points, size_t count)
			{
				std::vector<wxPoint> pointsBuffer = {points, points + count};
				m_DC->DrawSpline(static_cast<int>(pointsBuffer.size()), pointsBuffer.data());
			}

			template<size_t N>
			void DrawSpline(const Point(&points)[N])
			{
				std::array<wxPoint, N> pointsBuffer;
				std::copy_n(std::begin(points), N, pointsBuffer.begin());

				m_DC->DrawSpline(static_cast<int>(pointsBuffer.size()), pointsBuffer.data());
			}

			template<size_t N>
			void DrawSpline(const std::array<Point, N>& points)
			{
				std::array<wxPoint, N> pointsBuffer;
				std::copy_n(std::begin(points), N, pointsBuffer.begin());

				m_DC->DrawSpline(static_cast<int>(pointsBuffer.size()), pointsBuffer.data());
			}

			void DrawPoint(const Point& pos)
			{
				m_DC->DrawPoint(pos);
			}

			void DrawRectangle(const Point& pos, const Size& size)
			{
				m_DC->DrawRectangle(pos, size);
			}
			void DrawRectangle(const Rect& rect)
			{
				m_DC->DrawRectangle(rect);
			}
			void DrawRoundedRectangle(const Point& pos, const Size& size, double radius)
			{
				m_DC->DrawRoundedRectangle(pos, size, radius);
			}
			void DrawRoundedRectangle(const Rect& rect, double radius)
			{
				m_DC->DrawRoundedRectangle(rect, radius);
			}

			void DrawGradientLinear(const Rect& rect, const Color& startColor, const Color& endColor, Direction direction)
			{
				m_DC->GradientFillLinear(rect, startColor, endColor, static_cast<wxDirection>(direction));
			}
			void DrawGradientConcentric(const Rect& rect, const Color& startColor, const Color& endColor)
			{
				m_DC->GradientFillConcentric(rect, startColor, endColor);
			}
			void DrawGradientConcentric(const Rect& rect, const Point& center, const Color& startColor, const Color& endColor)
			{
				m_DC->GradientFillConcentric(rect, startColor, endColor, center);
			}

			void DrawCrossHair(const Point& pos)
			{
				m_DC->CrossHair(pos);
			}
			void DrawCrossHair(const Point& pos, const Size& size)
			{
				// Horizontal
				DrawLine({pos.GetX() - size.GetWidth(), pos.GetY()}, {pos.GetX() + size.GetWidth(), pos.GetY()});

				// Vertical
				DrawLine({pos.GetX(), pos.GetY() + size.GetHeight()}, {pos.GetX(), pos.GetY() - size.GetHeight()});
			}
			void DrawCrossHair(const Rect& rect)
			{
				DrawCrossHair(rect.GetPosition(), rect.GetSize());
			}
			void DrawCheckMark(const Rect& rect)
			{
				m_DC->DrawCheckMark(rect);
			}

			void DrawText(const Point& pos, const String& text)
			{
				m_DC->DrawText(text, pos);
			}
			Rect DrawLabel(const Rect& rect, const String& text, const Bitmap& bitmap, FlagSet<Alignment> alignment = Alignment::Left|Alignment::Top, size_t acceleratorIndex = String::npos)
			{
				wxRect boundingBox;
				m_DC->DrawLabel(text, bitmap.ToWxBitmap(), rect, alignment.ToInt(), acceleratorIndex != String::npos ? static_cast<int>(acceleratorIndex) : -1, &boundingBox);
				return boundingBox;
			}
			Rect DrawLabel(const Rect& rect, const String& text, FlagSet<Alignment> alignment = Alignment::Left|Alignment::Top, size_t acceleratorIndex = String::npos)
			{
				wxRect boundingBox;
				m_DC->DrawLabel(text, wxNullBitmap, rect, alignment.ToInt(), acceleratorIndex != String::npos ? static_cast<int>(acceleratorIndex) : -1, &boundingBox);
				return boundingBox;
			}

			bool CanDrawBitmap() const
			{
				return m_DC->CanDrawBitmap();
			}
			void DrawIcon(const Icon& icon, const Point& pos)
			{
				m_DC->DrawIcon(icon.ToWxIcon(), pos);
			}
			void DrawBitmap(const Bitmap& bitmap, const Point& pos)
			{
				m_DC->DrawBitmap(bitmap.ToWxBitmap(), pos, false);
			}

			void FloodFill(const Point& pos, const Color& color, FloodFillMode fillMode)
			{
				m_DC->FloodFill(pos, color, static_cast<wxFloodFillStyle>(fillMode));
			}

			// Clipping region functions
			void SetClippingRegion(const Rect& rect)
			{
				m_DC->SetClippingRegion(rect);
			}
			void SetDeviceClippingRegion(const Region& region)
			{
				m_DC->SetDeviceClippingRegion(region.ToWxRegion());
			}
			void ResetClippingRegion()
			{
				m_DC->DestroyClippingRegion();
			}

			std::optional<Rect> GetClippingBox() const
			{
				wxRect rect;
				if (m_DC->GetClippingBox(rect))
				{
					return rect;
				}
				return {};
			}

			// Text/character extent functions
			bool CanGetTextExtent() const
			{
				return m_DC->CanGetTextExtent();
			}

			Size GetCharSize() const
			{
				return {m_DC->GetCharWidth(), m_DC->GetCharHeight()};
			}
			int GetCharWidth() const
			{
				return m_DC->GetCharWidth();
			}
			int GetCharHeight() const
			{
				return m_DC->GetCharHeight();
			}
			FontMetrics GetFontMetrics() const
			{
				return m_DC->GetFontMetrics();
			}

			Size GetTextExtent(const String& text) const
			{
				return m_DC->GetTextExtent(text);
			}
			TextExtent GetTextExtent(const String& text, const Font& font) const
			{
				wxSize size;
				wxFontMetrics fontMetrics = m_DC->GetFontMetrics();
				m_DC->GetTextExtent(text, &size.x, &size.y, &fontMetrics.descent, &fontMetrics.externalLeading, &font.ToWxFont());
				return {size, fontMetrics};
			}

			Size GetMultiLineTextExtent(const String& text) const
			{
				return m_DC->GetMultiLineTextExtent(text);
			}
			TextExtent GetMultiLineTextExtent(const String& text, const Font& font) const
			{
				wxSize size;
				wxFontMetrics fontMetrics = m_DC->GetFontMetrics();
				m_DC->GetMultiLineTextExtent(text, &size.x, &size.y, &fontMetrics.height, &font.ToWxFont());
				return {size, fontMetrics};
			}

			std::vector<int> GetPartialTextExtent(const String& text) const
			{
				wxArrayInt widths;
				if (m_DC->GetPartialTextExtents(text, widths))
				{
					return widths;
				}
				return {};
			}

			// Text properties functions
			bool IsBackgroundTransparent() const
			{
				return m_DC->GetBackgroundMode() == wxBRUSHSTYLE_TRANSPARENT;
			}
			void SetBackgroundTransparen(bool isTransparent = true)
			{
				m_DC->SetBackgroundMode(isTransparent ? wxBRUSHSTYLE_TRANSPARENT : wxBRUSHSTYLE_SOLID);
			}

			UI::LayoutDirection GetLayoutDirection() const
			{
				using UI::LayoutDirection;

				switch (m_DC->GetLayoutDirection())
				{
					case wxLayout_LeftToRight:
					{
						return LayoutDirection::LeftToRight;
					}
					case wxLayout_RightToLeft:
					{
						return LayoutDirection::RightToLeft;
					}
				};
				return LayoutDirection::Default;
			}
			void SetLayoutDirection(UI::LayoutDirection layoutDirection)
			{
				using UI::LayoutDirection;

				switch (layoutDirection)
				{
					case LayoutDirection::LeftToRight:
					{
						m_DC->SetLayoutDirection(wxLayout_LeftToRight);
						break;
					}
					case LayoutDirection::RightToLeft:
					{
						m_DC->SetLayoutDirection(wxLayout_RightToLeft);
						break;
					}
					default:
					{
						m_DC->SetLayoutDirection(wxLayout_Default);
						break;
					}
				};
			}

			Font GetFont() const
			{
				return m_DC->GetFont();
			}
			void SetFont(const Font& font)
			{
				m_DC->SetFont(font.ToWxFont());
			}

			Color GetTextForeground() const
			{
				return m_DC->GetTextForeground();
			}
			void SetTextForeground(const Color& color)
			{
				m_DC->SetTextForeground(color);
			}

			Color GetTextBackground() const
			{
				return m_DC->GetTextBackground();
			}
			void SetTextBackground(const Color& color)
			{
				m_DC->SetTextBackground(color);
			}

			// Bounding box functions
			Rect GetBoundingBox() const
			{
				Rect rect;
				rect.SetTopLeft({m_DC->MinX(), m_DC->MinY()});
				rect.SetRightBottom({m_DC->MaxX(), m_DC->MaxY()});

				return rect;
			}
			void AddToBoundingBox(const Point& point)
			{
				m_DC->CalcBoundingBox(point.GetX(), point.GetY());
			}
			void ResetBoundingBox() const
			{
				m_DC->ResetBoundingBox();
			}

			// Page and document start/end functions
			void StartDocument(const String& message)
			{
				m_DC->StartDoc(message);
			}
			void EndDocument()
			{
				m_DC->EndDoc();
			}

			void StartPage()
			{
				m_DC->StartPage();
			}
			void EndPage()
			{
				m_DC->EndPage();
			}

			// Bit-Block Transfer operations (blit)
			bool Blit(const Point& destination, const Size& size, GDICanvas& sourceCanvas, const Point& source, GDILogicalFunction mode = GDILogicalFunction::Copy)
			{
				if (sourceCanvas)
				{
					return m_DC->Blit(destination, size, sourceCanvas.m_DC, source, static_cast<wxRasterOperationMode>(mode), false, wxDefaultPosition);
				}
				return false;
			}
			bool StretchBlit(const Rect& destination, const Size& size, GDICanvas& sourceCanvas, const Rect& source, GDILogicalFunction mode = GDILogicalFunction::Copy)
			{
				if (sourceCanvas)
				{
					return m_DC->StretchBlit(destination.GetPosition(), destination.GetSize(), sourceCanvas.m_DC, source.GetPosition(), source.GetSize(), static_cast<wxRasterOperationMode>(mode), false, wxDefaultPosition);
				}
				return false;
			}

			// Background/foreground brush and pen
			Brush GetBackgroundBrush() const
			{
				return m_DC->GetBackground();
			}
			void SetBackgroundBrush(const Brush& brush)
			{
				m_DC->SetBackground(brush.ToWxBrush());
			}

			Brush GetBrush() const
			{
				return m_DC->GetBrush();
			}
			void SetBrush(const Brush& brush)
			{
				m_DC->SetBrush(brush.ToWxBrush());
			}

			Pen GetPen() const
			{
				return m_DC->GetPen();
			}
			void SetPen(const Pen& pen)
			{
				m_DC->SetPen(pen.ToWxPen());
			}

			// Transformation matrix
			bool CanUseTransformMatrix() const
			{
				return m_DC->CanUseTransformMatrix();
			}
			wxAffineMatrix2D GetTransformMatrix() const
			{
				return m_DC->GetTransformMatrix();
			}
			bool SetTransformMatrix(const wxAffineMatrix2D& matrix)
			{
				return m_DC->SetTransformMatrix(matrix);
			}
			void ResetTransformMatrix()
			{
				m_DC->ResetTransformMatrix();
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDICanvas& operator=(const GDICanvas&) = delete;
	};
}
