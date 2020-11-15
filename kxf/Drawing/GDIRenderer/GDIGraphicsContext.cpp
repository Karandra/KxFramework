#include "stdafx.h"
#include "GDIGraphicsContext.h"
#include "GDIGraphicsTexture.h"
#include "GDIGraphicsMatrix.h"
#include "GDIGraphicsBrush.h"
#include "GDIGraphicsPen.h"
#include "GDIGraphicsFont.h"
#include "GDIAction.h"
#include "../GraphicsRenderer/GraphicsAction.h"
#include "Private/Common.h"
#include <wx/msw/dc.h>

namespace kxf
{
	GDIGraphicsContext::GDIGraphicsContext(GDIGraphicsRenderer& rendrer, wxDC& dc)
		:m_Renderer(&rendrer), m_DC(dc)
	{
		m_DC.SetMapMode(GDIMappingMode::Text);
	}

	// Feature support
	FlagSet<GraphicsContextFeature> GDIGraphicsContext::GetSupportedFeatures() const
	{
		if (m_DC)
		{
			FlagSet<GraphicsContextFeature> features;
			features.Add(GraphicsContextFeature::ClippingRegion);
			features.Add(GraphicsContextFeature::TransformationMatrix, m_DC.CanUseTransformMatrix());
			features.Add(GraphicsContextFeature::DrawText);
			features.Add(GraphicsContextFeature::DrawShape);
			features.Add(GraphicsContextFeature::DrawTexture, m_DC.CanDrawBitmap());
			features.Add(GraphicsContextFeature::TextMeasurement, m_DC.CanGetTextExtent());

			return features;
		}
		return {};
	}

	// Transformation matrix
	std::shared_ptr<IGraphicsMatrix> GDIGraphicsContext::GetTransform() const
	{
		if (m_DC.CanUseTransformMatrix())
		{
			return std::make_shared<GDIGraphicsMatrix>(*m_Renderer, m_DC.GetTransformMatrix());
		}
		return {};
	}
	bool GDIGraphicsContext::SetTransform(std::shared_ptr<IGraphicsMatrix> transform)
	{
		if (m_DC.CanUseTransformMatrix() && transform)
		{
			if (auto affineMatrix = Drawing::Private::ToAffineMatrix2D(*transform))
			{
				return m_DC.SetTransformMatrix(*affineMatrix);
			}
		}
		return false;
	}

	bool GDIGraphicsContext::TransformInvert()
	{
		if (m_DC.CanUseTransformMatrix())
		{
			auto matrix = m_DC.GetTransformMatrix();
			if (matrix.Invert())
			{
				m_DC.SetTransformMatrix(matrix);
				return true;
			}
		}
		return false;
	}
	void GDIGraphicsContext::TransformRotate(Angle angle)
	{
		if (m_DC.CanUseTransformMatrix())
		{
			auto matrix = m_DC.GetTransformMatrix();
			matrix.Rotate(angle.ToRadians());
			m_DC.SetTransformMatrix(matrix);
		}
	}
	void GDIGraphicsContext::TransformScale(float xScale, float yScale)
	{
		if (m_DC.CanUseTransformMatrix())
		{
			auto matrix = m_DC.GetTransformMatrix();
			matrix.Scale(xScale, yScale);
			m_DC.SetTransformMatrix(matrix);
		}
	}
	void GDIGraphicsContext::TransformTranslate(const Size& dxy)
	{
		if (m_DC.CanUseTransformMatrix())
		{
			auto matrix = m_DC.GetTransformMatrix();
			matrix.Translate(dxy.GetWidth(), dxy.GetHeight());
			m_DC.SetTransformMatrix(matrix);
		}
	}
	void GDIGraphicsContext::TransformConcat(const IGraphicsMatrix& matrix)
	{
		if (m_DC.CanUseTransformMatrix())
		{
			if (auto affineMatrix = Drawing::Private::ToAffineMatrix2D(matrix))
			{
				auto matrix = m_DC.GetTransformMatrix();
				matrix.Concat(*affineMatrix);
				m_DC.SetTransformMatrix(matrix);
			}
		}
	}

	// Pen and brush functions
	std::shared_ptr<IGraphicsPen> GDIGraphicsContext::GetPen() const
	{
		return std::make_shared<GDIGraphicsPen>(*m_Renderer, m_DC.GetPen());
	}
	void GDIGraphicsContext::SetPen(std::shared_ptr<IGraphicsPen> pen)
	{
		if (pen && *pen)
		{
			m_DC.SetPen(m_Renderer->ToGDIPen(*pen));
		}
		else
		{
			m_DC.SetPen({});
		}
	}

	std::shared_ptr<IGraphicsBrush> GDIGraphicsContext::GetBrush() const
	{
		if (GDIBrush brush = m_DC.GetBrush())
		{
			if (brush.IsSolid())
			{
				return std::make_shared<GDIGraphicsSolidBrush>(*m_Renderer, std::move(brush));
			}
			else if (brush.IsHatch())
			{
				return std::make_shared<GDIGraphicsHatchBrush>(*m_Renderer, std::move(brush));
			}

			// We can't restore any of other brush styles from the GDI brush so just return the saved one.
			return m_SavedBrush;
		}
		return {};
	}
	void GDIGraphicsContext::SetBrush(std::shared_ptr<IGraphicsBrush> brush)
	{
		if (brush && *brush)
		{
			if (GDIBrush gdiBrush = m_Renderer->ToGDIBrush(*brush))
			{
				m_DC.SetBrush(gdiBrush);
			}
			else
			{
				// We can't reliably save this graphical brush as a GDI brush, so save it as is for later use.
				m_DC.SetBrush({});
				m_SavedBrush = std::move(brush);
			}
		}
		else
		{
			m_SavedBrush = {};
			m_DC.SetBrush({});
		}
	}

	// Texture functions
	void GDIGraphicsContext::DrawTexture(const IGraphicsTexture& texture, const RectF& rect)
	{
		if (m_DC.CanDrawBitmap())
		{
			if (auto gdiTexture = texture.QueryInterface<GDIGraphicsTexture>())
			{
				if (gdiTexture->GetSize() == rect.GetSize())
				{
					m_DC.DrawBitmap(gdiTexture->Get(), rect.GetPosition());
				}
				else
				{
					m_DC.DrawBitmap(gdiTexture->Get().ToImage().RescaleThis(rect.GetSize(), m_InterpolationQuality).ToBitmap(), rect.GetPosition());
				}
			}
		}
	}
	void GDIGraphicsContext::DrawTexture(const Image& image, const RectF& rect)
	{
		if (m_DC.CanDrawBitmap())
		{
			if (SizeF(image.GetSize()) != rect.GetSize())
			{
				m_DC.DrawBitmap(image.Rescale(rect.GetSize(), m_InterpolationQuality).ToBitmap(), rect.GetPosition());
			}
			else
			{
				m_DC.DrawBitmap(image.ToBitmap(), rect.GetPosition());
			}
		}
	}
	void GDIGraphicsContext::DrawTexture(const Bitmap& bitmap, const RectF& rect)
	{
		if (m_DC.CanDrawBitmap())
		{
			if (SizeF(bitmap.GetSize()) != rect.GetSize())
			{
				m_DC.DrawBitmap(bitmap.ToImage().RescaleThis(rect.GetSize(), m_InterpolationQuality).ToBitmap(), rect.GetPosition());
			}
			else
			{
				m_DC.DrawBitmap(bitmap, rect.GetPosition());
			}
		}
	}

	// Text functions
	std::shared_ptr<IGraphicsFont> GDIGraphicsContext::GetFont() const
	{
		return std::make_shared<GDIGraphicsFont>(*m_Renderer, m_DC.GetFont(), m_DC.GetTextForeground());
	}
	void GDIGraphicsContext::SetFont(std::shared_ptr<IGraphicsFont> font)
	{
		if (font)
		{
			if (auto gdiFont = font->QueryInterface<GDIGraphicsFont>())
			{
				m_DC.SetFont(gdiFont->Get());
			}
			m_DC.SetTextForeground(font->GetColor());
		}
		else
		{
			m_DC.SetFont({});
			m_DC.SetTextForeground({});
		}
	}

	GraphicsTextExtent GDIGraphicsContext::GetTextExtent(const String& text) const
	{
		if (text.ContainsAnyOfCharacters(wxS("\r\n")))
		{
			return m_DC.GetMultiLineTextExtent(text, m_DC.GetFont());
		}
		else
		{
			return m_DC.GetTextExtent(text, m_DC.GetFont());
		}
	}
	std::vector<float> GDIGraphicsContext::GetPartialTextExtent(const String& text) const
	{
		wxArrayInt gdiWidths;
		if (m_DC.ToWxDC().GetPartialTextExtents(text, gdiWidths))
		{
			std::vector<float> widths;
			widths.resize(gdiWidths.size());
			std::copy(gdiWidths.begin(), gdiWidths.end(), widths.begin());

			return widths;
		}
		return {};
	}

	void GDIGraphicsContext::DrawText(const String& text, const PointF& point)
	{
		m_DC.DrawText(text, point);
	}
	void GDIGraphicsContext::DrawText(const String& text, const PointF& point, const IGraphicsBrush& brush)
	{
		GDIAction::ChangeBackground changeBackground(m_DC);
		if (brush)
		{
			changeBackground.Set(m_Renderer->ToGDIBrush(brush));
		}
		m_DC.DrawText(text, point);
	}

	void GDIGraphicsContext::DrawRotatedText(const String& text, const PointF& point, Angle angle)
	{
		m_DC.DrawRotatedText(text, point, angle);
	}
	void GDIGraphicsContext::DrawRotatedText(const String& text, const PointF& point, Angle angle, const IGraphicsBrush& brush)
	{
		GDIAction::ChangeBackground changeBackground(m_DC);
		if (brush)
		{
			changeBackground.Set(m_Renderer->ToGDIBrush(brush));
		}
		m_DC.DrawRotatedText(text, point, angle);
	}

	RectF GDIGraphicsContext::DrawLabel(const String& text, const RectF& rect, const IGraphicsTexture& icon, FlagSet<Alignment> alignment, size_t acceleratorIndex)
	{
		if (m_DC.CanDrawBitmap())
		{
			if (auto gdiTexture = icon.QueryInterface<GDIGraphicsTexture>())
			{
				return m_DC.DrawLabel(text, rect, gdiTexture->Get(), alignment, acceleratorIndex);
			}
		}
		return m_DC.DrawLabel(text, rect, {}, alignment, acceleratorIndex);
	}
	RectF GDIGraphicsContext::DrawLabel(const String& text, const RectF& rect, FlagSet<Alignment> alignment, size_t acceleratorIndex)
	{
		return m_DC.DrawLabel(text, rect, alignment, acceleratorIndex);
	}

	// Drawing functions
	void GDIGraphicsContext::Clear(const IGraphicsBrush& brush)
	{
		GDIAction::ChangeBackground changeBackground(m_DC);
		if (brush)
		{
			changeBackground.Set(m_Renderer->ToGDIBrush(brush));
		}

		m_DC.Clear();
	}
	void GDIGraphicsContext::DrawCircle(const Point& pos, float radius)
	{
		m_DC.DrawCircle(pos, radius);
	}
	void GDIGraphicsContext::DrawEllipse(const RectF& rect)
	{
		m_DC.DrawEllipse(rect);
	}
	void GDIGraphicsContext::DrawRectangle(const RectF& rect)
	{
		m_DC.DrawRectangle(rect);
	}
	void GDIGraphicsContext::DrawRoundedRectangle(const RectF& rect, float radius)
	{
		m_DC.DrawRoundedRectangle(rect, radius);
	}
	void GDIGraphicsContext::DrawLine(const PointF& point1, const PointF& point2)
	{
		m_DC.DrawLine(point1, point2);
	}
	void GDIGraphicsContext::DrawPolyLine(const PointF* points, size_t count)
	{
		std::vector<Point> gdiPoints;
		gdiPoints.resize(count);
		std::copy_n(points, count, gdiPoints.begin());

		m_DC.DrawPolyLine(gdiPoints.data(), gdiPoints.size());
	}
	void GDIGraphicsContext::DrawDisconnectedLines(const PointF* startPoints, const PointF* endPoints, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			m_DC.DrawLine(startPoints[i], endPoints[i]);
		}
	}

	// Bounding box functions
	RectF GDIGraphicsContext::GetBoundingBox() const
	{
		return m_DC.GetBoundingBox();
	}
	void GDIGraphicsContext::CalcBoundingBox(const PointF& point)
	{
		m_DC.CalcBoundingBox(point);
	}
	void GDIGraphicsContext::ResetBoundingBox()
	{
		m_DC.ResetBoundingBox();
	}
}

namespace kxf
{
	std::shared_ptr<IGraphicsTexture> GDIGraphicsMemoryContext::GetSelectedTexture() const
	{
		return m_Texture;
	}
	void GDIGraphicsMemoryContext::SelectTexture(std::shared_ptr<IGraphicsTexture> texture)
	{
		m_Texture = std::move(texture);
		if (m_Texture)
		{
			m_MemoryDC.SelectObject(m_Texture->QueryInterface<GDIGraphicsTexture>()->Get().ToWxBitmap());
		}
		else
		{
			m_MemoryDC.SelectObject(wxNullBitmap);
		}
	}
	std::shared_ptr<IGraphicsTexture> GDIGraphicsMemoryContext::UnselectTexture()
	{
		m_MemoryDC.SelectObject(wxNullBitmap);
		return std::move(m_Texture);
	}
}

namespace kxf
{
	GDIGraphicsBufferedContext::GDIGraphicsBufferedContext(GDIGraphicsRenderer& rendrer, const SizeF& size, FlagSet<GDIBufferedContextFlag> flags)
		: GDIGraphicsContext(rendrer, m_BufferedDC), m_BufferedDC(nullptr, size, flags.ToInt())
	{
	}
	GDIGraphicsBufferedContext::GDIGraphicsBufferedContext(GDIGraphicsRenderer& rendrer, std::shared_ptr<IGraphicsTexture> texture, FlagSet<GDIBufferedContextFlag> flags)
		:GDIGraphicsContext(rendrer, m_BufferedDC), m_BufferedDC(nullptr, texture->QueryInterface<GDIGraphicsTexture>()->Get().ToWxBitmap(), flags.ToInt()), m_Texture(std::move(texture))
	{
	}
}
