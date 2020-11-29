#include "stdafx.h"
#include "HTMLRenderer.h"
#include "../View.h"
#include "../Node.h"
#include "../Column.h"
#include "kxf/UI/Controls/HTMLWindow.h"
#include "kxf/Drawing/GDIRenderer/GDIGraphicsContext.h"
#include <wx/html/htmprint.h>

namespace
{
	// For some unknown reason scale of '1.0' renders text too large.
	// Scaling it to '0.8' solves this.
	constexpr double g_UserScale = 0.8;

	class DCUserScaleSaver final
	{
		private:
			kxf::GDIContext& m_DC;
			kxf::SizeD m_UserScale;

		public:
			DCUserScaleSaver(kxf::GDIContext& dc)
				:m_DC(dc)
			{
				m_UserScale = m_DC.GetUserScale();
			}
			~DCUserScaleSaver()
			{
				m_DC.SetUserScale(m_UserScale);
			}
	};
}

namespace kxf::UI::DataView
{
	bool HTMLRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		m_ContentHTML.clear();

		if (m_Value.FromAny(value))
		{
			m_ContentHTML = kxf::UI::HTMLWindow::ProcessPlainText(m_Value.GetText());
			return true;
		}
		return false;
	}
	ToolTip HTMLRenderer::CreateToolTip() const
	{
		return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
	}

	void HTMLRenderer::PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, GDIContext& dc, const Rect& cellRect) const
	{
		htmlRenderer.SetDC(&dc.ToWxDC(), g_UserScale * m_PixelScale, g_UserScale * m_FontScale);

		// Size
		Size size = cellRect.GetSize();
		size.IncTo(GetCellSize());
		htmlRenderer.SetSize(size.GetWidth(), size.GetHeight());

		// Set HTML
		htmlRenderer.SetHtmlText(m_ContentHTML);

		// Setup fonts. This needs to be done after a call to 'wxHtmlDCRenderer::SetHtmlText',
		// otherwise text scales really weirdly.
		int pointSize = 0;
		kxf::String normalFace;
		kxf::String fixedFace;
		if (kxf::UI::HTMLWindow::SetupFontsUsing(dc.GetFont(), normalFace, fixedFace, pointSize))
		{
			htmlRenderer.SetFonts(normalFace, fixedFace);
			htmlRenderer.SetStandardFonts(pointSize, normalFace, fixedFace);
		}
	}
	void HTMLRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		if (m_Value.HasText())
		{
			IGraphicsContext& gc = GetGraphicsContext();
			if (auto gdi = gc.QueryInterface<GDIGraphicsContext>())
			{
				DCUserScaleSaver userScaleSaver(gdi->Get());
				GDIAction::Clip clip(gdi->Get(), cellRect.Clone().Deflate(0, GetRenderEngine().FromDIPY(2)));

				// Render text
				wxHtmlDCRenderer htmlRenderer;
				PrepareRenderer(htmlRenderer, gdi->Get(), cellRect);
				htmlRenderer.Render(cellRect.GetX(), cellRect.GetY(), m_VisibleCellFrom, m_VisibleCellTo);
			}
		}
	}
	Size HTMLRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			return GetRenderEngine().GetMultilineTextExtent(m_Value.GetText());
		}
		return Size(0, 0);
	}
}
