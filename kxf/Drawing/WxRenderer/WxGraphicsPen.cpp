#include "stdafx.h"
#include "WxGraphicsPen.h"
#include "../Image.h"
#include "../GraphicsRenderer/IGraphicsBrush.h"
#include "../GraphicsRenderer/IGraphicsTexture.h"

namespace kxf
{
	void WxGraphicsPen::AssignBrushData()
	{
		if (m_Brush)
		{
			if (auto textureBrush = m_Brush->QueryInterface<IGraphicsTextureBrush>())
			{
				if (auto texture = textureBrush->GetTexture())
				{
					m_Pen.SetStipple(texture->ToImage().ToBitmap());
				}
			}
			else if (auto hatchBrush = m_Brush->QueryInterface<IGraphicsHatchBrush>())
			{
				m_Pen.SetHatchStyle(hatchBrush->GetHatchStyle());
			}
		}
		else
		{
			m_Pen.SetStipple({});
		}
		ValidatePen();
	}
	void WxGraphicsPen::ValidatePen()
	{
		if (!m_Renderer->CanDrawNullBitmap())
		{
			if (!m_Pen.GetStipple())
			{
				m_Pen.ToWxPen().SetStipple(m_Renderer->GetNullBitmap().ToWxBitmap());
			}
		}
	}

	PenStyle WxGraphicsPen::GetStyle() const
	{
		if (m_Pen.IsSolid())
		{
			return PenStyle::Solid;
		}
		else if (m_Pen.IsHatch())
		{
			return PenStyle::Hatch;
		}
		else if (m_Pen.IsDash())
		{
			return PenStyle::Dash;
		}
		else if (m_Pen.GetStipple())
		{
			return PenStyle::Texture;
		}
		return PenStyle::None;
	}
	void WxGraphicsPen::SetStyle(PenStyle style)
	{
		switch (style)
		{
			case PenStyle::Solid:
			{
				m_Pen.SetSolid();
				break;
			}
			case PenStyle::Hatch:
			{
				m_Pen.SetHatchStyle(HatchStyle::Horizontal);
				break;
			}
			case PenStyle::Dash:
			{
				m_Pen.SetDashStyle(DashStyle::Dash);
				break;
			}
			case PenStyle::Texture:
			{
				m_Pen.ToWxPen().SetStyle(wxPENSTYLE_STIPPLE);
				break;
			}
			default:
			{
				m_Pen.SetTransparent();
				break;
			}
		};
		Invalidate();
	}
}
