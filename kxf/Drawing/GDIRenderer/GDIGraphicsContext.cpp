#include "stdafx.h"
#include "GDIGraphicsContext.h"
#include "GDIGraphicsTexture.h"
#include "GDIGraphicsBrush.h"
#include "GDIGraphicsPen.h"
#include "GDIGraphicsFont.h"
#include "GDIAction.h"
#include "../GraphicsRenderer/GraphicsAction.h"
#include <wx/msw/dc.h>

namespace
{
	using namespace kxf;

	class ChangeTextParameters final
	{
		private:
			GDIAction::ChangeFont m_Font;
			GDIAction::ChangeTextForeground m_TextForeground;

		public:
			ChangeTextParameters(GDIContext& dc, const IGraphicsFont& font, const IGraphicsBrush& brush)
				:m_Font(dc), m_TextForeground(dc)
			{
				if (font)
				{
					if (auto gdiFont = font.QueryInterface<GDIGraphicsFont>())
					{
						m_Font.Set(gdiFont->Get());
					}
					else
					{
						m_Font.Set(font.ToFont());
					}
				}
				if (auto solidBrush = brush.QueryInterface<IGraphicsSolidBrush>())
				{
					m_TextForeground.Set(solidBrush->GetColor());
				}
			}
	};
	class ChangeDrawParameters final
	{
		private:
			GDIAction::ChangePen m_Pen;
			GDIAction::ChangeBrush m_Brush;

		public:
			ChangeDrawParameters(GDIContext& dc, const IGraphicsBrush& brush, const IGraphicsPen& pen)
				:m_Pen(dc), m_Brush(dc)
			{
				if (brush)
				{
					m_Brush.Set(brush.QueryInterface<GDIGraphicsBrush>()->Get());
				}
				if (pen)
				{
					m_Pen.Set(pen.QueryInterface<GDIGraphicsPen>()->Get());
				}
			}
	};

	enum class TextKind
	{
		Simple,
		Rotated,
		Label
	};
	RectF DoDrawText(GDIContext& dc,
					 TextKind kind,
					 const String& text,
					 const RectF& rect,
					 Angle angle,
					 const IGraphicsTexture& icon,
					 const IGraphicsFont& font,
					 const IGraphicsBrush& brush,
					 FlagSet<Alignment> alignment,
					 size_t acceleratorIndex)
	{
		switch (kind)
		{
			case TextKind::Simple:
			{
				if (!text.IsEmpty())
				{
					ChangeTextParameters textParametrs(dc, font, brush);
					dc.DrawText(text, rect.GetPosition());
				}
				break;
			}
			case TextKind::Rotated:
			{
				if (!text.IsEmpty())
				{
					ChangeTextParameters textParametrs(dc, font, brush);
					dc.DrawRotatedText(text, rect.GetPosition(), angle);
				}
				break;
			}
			case TextKind::Label:
			{
				if (!rect.IsEmpty() && (!text.IsEmpty() || icon))
				{
					ChangeTextParameters textParametrs(dc, font, brush);

					if (icon && dc.CanDrawBitmap())
					{
						return dc.DrawLabel(text, rect, icon.QueryInterface<GDIGraphicsTexture>()->Get(), alignment, acceleratorIndex);
					}
					return dc.DrawLabel(text, rect, {}, alignment, acceleratorIndex);
				}
				break;
			}
		};
		return {};
	}
}

namespace kxf
{
	void GDIGraphicsContext::SetupDC()
	{
		if (m_DC)
		{
			m_DC.SetMapMode(GDIMappingMode::Text);
			m_DC.SetTextBackground(Drawing::GetStockColor(StockColor::Transparent));
		}
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
			features.Add(GraphicsContextFeature::BoundingBox);

			return features;
		}
		return {};
	}

	// Transformation matrix
	AffineMatrixF GDIGraphicsContext::GetTransform() const
	{
		if (m_DC.CanUseTransformMatrix())
		{
			return m_DC.GetTransformMatrix();
		}
		return {};
	}
	bool GDIGraphicsContext::SetTransform(const AffineMatrixF& transform)
	{
		if (m_DC.CanUseTransformMatrix())
		{
			return m_DC.SetTransformMatrix(transform);
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
			matrix.Rotate(angle);
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
	void GDIGraphicsContext::TransformTranslate(const SizeF& dxy)
	{
		if (m_DC.CanUseTransformMatrix())
		{
			auto matrix = m_DC.GetTransformMatrix();
			matrix.Translate(static_cast<int>(dxy.GetWidth()), static_cast<int>(dxy.GetHeight()));
			m_DC.SetTransformMatrix(matrix);
		}
	}
	void GDIGraphicsContext::TransformConcat(const AffineMatrixF& transform)
	{
		if (m_DC.CanUseTransformMatrix())
		{
			auto matrix = m_DC.GetTransformMatrix();
			matrix.Concat(transform);
			m_DC.SetTransformMatrix(matrix);
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
				// We can't really convert this graphical brush to a GDI brush, so save it as is for later use.
				// TODO: Make use of that saved brush somehow.
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
		if (m_DC.CanDrawBitmap() && !rect.IsEmpty())
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
			else
			{
				GDIGraphicsContext::DrawTexture(texture.ToImage(), rect);
			}
		}
	}
	void GDIGraphicsContext::DrawTexture(const Image& image, const RectF& rect)
	{
		if (m_DC.CanDrawBitmap() && !rect.IsEmpty())
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
	void GDIGraphicsContext::DrawTexture(const GDIBitmap& bitmap, const RectF& rect)
	{
		if (m_DC.CanDrawBitmap() && !rect.IsEmpty())
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
		return std::make_shared<GDIGraphicsFont>(*m_Renderer, m_DC.GetFont());
	}
	void GDIGraphicsContext::SetFont(std::shared_ptr<IGraphicsFont> font)
	{
		if (font)
		{
			if (auto gdiFont = font->QueryInterface<GDIGraphicsFont>())
			{
				m_DC.SetFont(gdiFont->Get());
			}
			else
			{
				m_DC.SetFont(font->ToFont());
			}
		}
		else
		{
			m_DC.SetFont({});
		}
	}

	std::shared_ptr<IGraphicsBrush> GDIGraphicsContext::GetFontBrush() const
	{
		return std::make_shared<GDIGraphicsSolidBrush>(*m_Renderer, m_DC.GetTextForeground());
	}
	void GDIGraphicsContext::SetFontBrush(std::shared_ptr<IGraphicsBrush> brush)
	{
		object_ptr<IGraphicsSolidBrush> solidBrush;
		if (brush && brush->QueryInterface(solidBrush))
		{
			m_DC.SetTextForeground(solidBrush->GetColor());
		}
		else
		{
			m_DC.SetTextForeground({});
		}
		m_DC.SetTextBackground(Drawing::GetStockColor(StockColor::Transparent));
	}

	SizeF GDIGraphicsContext::GetTextExtent(const String& text, const IGraphicsFont& font) const
	{
		if (!text.IsEmpty())
		{
			auto GetExtent = [&](const GDIFont& font)
			{
				if (text.ContainsAnyOfCharacters(wxS("\r\n")))
				{
					return m_DC.GetMultiLineTextExtent(text, font);
				}
				else
				{
					return m_DC.GetTextExtent(text, font);
				}
			};
			if (font)
			{
				return GetExtent(font.QueryInterface<GDIGraphicsFont>()->Get());
			}
			else
			{
				return GetExtent(m_DC.GetFont());
			}
		}
		return {};
	}
	FontMetricsF GDIGraphicsContext::GetFontMetrics(const IGraphicsFont& font) const
	{
		if (font)
		{
			ChangeTextParameters textParametrs(const_cast<GDIContext&>(m_DC), font, NullGraphicsBrush);
			return m_DC.GetFontMetrics();
		}
		else
		{
			return m_DC.GetFontMetrics();
		}
	}
	std::vector<float> GDIGraphicsContext::GetPartialTextExtent(const String& text, const IGraphicsFont& font) const
	{
		if (!text.IsEmpty())
		{
			GDIAction::ChangeFont changeFont(const_cast<GDIContext&>(m_DC));
			if (font)
			{
				changeFont.Set(font.QueryInterface<GDIGraphicsFont>()->Get());
			}

			wxArrayInt gdiWidths;
			if (m_DC.ToWxDC().GetPartialTextExtents(text, gdiWidths))
			{
				std::vector<float> widths;
				widths.resize(gdiWidths.size());
				std::copy(gdiWidths.begin(), gdiWidths.end(), widths.begin());

				return widths;
			}
		}
		return {};
	}

	void GDIGraphicsContext::DrawText(const String& text, const PointF& point, const IGraphicsFont& font, const IGraphicsBrush& brush)
	{
		DoDrawText(m_DC, TextKind::Simple, text, RectF(point, SizeF::UnspecifiedSize()), {}, NullGraphicsTexture, font, brush, {}, String::npos);
	}
	void GDIGraphicsContext::DrawRotatedText(const String& text, const PointF& point, Angle angle, const IGraphicsFont& font, const IGraphicsBrush& brush)
	{
		DoDrawText(m_DC, TextKind::Rotated, text, RectF(point, SizeF::UnspecifiedSize()), angle, NullGraphicsTexture, font, brush, {}, String::npos);
	}
	RectF GDIGraphicsContext::DrawLabel(const String& text, const RectF& rect, const IGraphicsTexture& icon, const IGraphicsFont& font, const IGraphicsBrush& brush, FlagSet<Alignment> alignment, size_t acceleratorIndex)
	{
		return DoDrawText(m_DC, TextKind::Label, text, rect, {}, icon, font, brush, alignment, acceleratorIndex);
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
	void GDIGraphicsContext::DrawCircle(const PointF& pos, float radius, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (radius != 0)
		{
			ChangeDrawParameters drawParameters(m_DC, brush, pen);
			m_DC.DrawCircle(pos, radius);
		}
	}
	void GDIGraphicsContext::DrawEllipse(const RectF& rect, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (!rect.IsEmpty())
		{
			ChangeDrawParameters drawParameters(m_DC, brush, pen);
			m_DC.DrawEllipse(rect);
		}
	}
	void GDIGraphicsContext::DrawRectangle(const RectF& rect, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (!rect.IsEmpty())
		{
			ChangeDrawParameters drawParameters(m_DC, brush, pen);
			m_DC.DrawRectangle(rect);
		}
	}
	void GDIGraphicsContext::DrawRoundedRectangle(const RectF& rect, float radius, const IGraphicsBrush& brush, const IGraphicsPen& pen)
	{
		if (!rect.IsEmpty())
		{
			ChangeDrawParameters drawParameters(m_DC, brush, pen);
			m_DC.DrawRoundedRectangle(rect, radius);
		}
	}
	void GDIGraphicsContext::DrawLine(const PointF& point1, const PointF& point2, const IGraphicsPen& pen)
	{
		ChangeDrawParameters drawParameters(m_DC, NullGraphicsBrush, pen);

		m_DC.DrawLine(point1, point2);
	}
	void GDIGraphicsContext::DrawPolyLine(const PointF* points, size_t count, const IGraphicsPen& pen)
	{
		if (count != 0)
		{
			ChangeDrawParameters drawParameters(m_DC, NullGraphicsBrush, pen);

			std::vector<Point> gdiPoints;
			gdiPoints.resize(count);
			std::copy_n(points, count, gdiPoints.begin());

			m_DC.DrawPolyLine(gdiPoints.data(), gdiPoints.size());
		}
	}
	void GDIGraphicsContext::DrawDisconnectedLines(const PointF* startPoints, const PointF* endPoints, size_t count, const IGraphicsPen& pen)
	{
		if (count != 0)
		{
			ChangeDrawParameters drawParameters(m_DC, NullGraphicsBrush, pen);

			for (size_t i = 0; i < count; i++)
			{
				m_DC.DrawLine(startPoints[i], endPoints[i]);
			}
		}
	}

	// Getting and setting parameters
	CompositionMode GDIGraphicsContext::GetCompositionMode() const
	{
		return m_CompositionMode;
	}
	void GDIGraphicsContext::SetCompositionMode(CompositionMode mode)
	{
		m_CompositionMode = mode;
		switch (mode)
		{
			case CompositionMode::Source:
			case CompositionMode::Over:
			{
				m_DC.SetLogicalFunction(GDILogicalFunction::Copy);
				break;
			}
			case CompositionMode::Add:
			{
				m_DC.SetLogicalFunction(GDILogicalFunction::Or);
				break;
			}
			case CompositionMode::Clear:
			{
				m_DC.SetLogicalFunction(GDILogicalFunction::Clear);
				break;
			}
			case CompositionMode::Dest:
			{
				m_DC.SetLogicalFunction(GDILogicalFunction::Nop);
				break;
			}
			case CompositionMode::Xor:
			{
				m_DC.SetLogicalFunction(GDILogicalFunction::Xor);
				break;
			}
			default:
			{
				m_CompositionMode = CompositionMode::None;
				break;
			}
		};
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
		SetupDC();
	}
	GDIGraphicsBufferedContext::GDIGraphicsBufferedContext(GDIGraphicsRenderer& rendrer, std::shared_ptr<IGraphicsTexture> texture, FlagSet<GDIBufferedContextFlag> flags)
		:GDIGraphicsContext(rendrer, m_BufferedDC), m_BufferedDC(nullptr, texture->QueryInterface<GDIGraphicsTexture>()->Get().ToWxBitmap(), flags.ToInt()), m_Texture(std::move(texture))
	{
		SetupDC();
	}
}
