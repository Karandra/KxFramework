#include "KxStdAfx.h"
#include "HTMLRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"
#include "KxFramework/KxHTMLWindow.h"
#include <wx/html/htmprint.h>

namespace KxDataView2
{
	bool HTMLRenderer::SetValue(const wxAny& value)
	{
		m_Value.Clear();
		m_ContentHTML.clear();

		if (m_Value.FromAny(value))
		{
			m_ContentHTML = KxHTMLWindow::ProcessPlainText(m_Value.GetText());
			return true;
		}
		return false;
	}
	ToolTip HTMLRenderer::CreateToolTip() const
	{
		return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
	}

	void HTMLRenderer::PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, wxDC& dc, const wxRect& cellRect) const
	{
		// For some unknown reason scale of '1.0' renders text too large.
		// Scaling it to '0.8' solves this.
		constexpr double userScale = 0.8;
		htmlRenderer.SetDC(&dc, userScale * m_PixelScale, userScale * m_FontScale);

		// Size
		wxSize size = cellRect.GetSize();
		size.IncTo(GetCellSize());
		htmlRenderer.SetSize(size.GetWidth(), size.GetHeight());

		// Content
		htmlRenderer.SetHtmlText(m_ContentHTML);

		const wxFont& font = dc.GetFont();
		htmlRenderer.SetFonts(font.GetFaceName(), wxS("Consolas"));
		htmlRenderer.SetStandardFonts(font.GetPointSize(), font.GetFaceName(), wxS("Consolas"));
	}
	void HTMLRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		if (m_Value.HasText())
		{
			// Prefer regular DC
			wxDC& dc = HasRegularDC() ? GetRegularDC() : GetGraphicsDC();
			wxHtmlDCRenderer htmlRenderer;
			PrepareRenderer(htmlRenderer, dc, cellRect);

			// Save user scale
			wxPoint2DDouble userScale(1.0,  1.0);
			dc.GetUserScale(&userScale.m_x, &userScale.m_y);

			// Render text
			htmlRenderer.Render(cellRect.GetX(), cellRect.GetY(), m_VisibleCellFrom, m_VisibleCellTo);

			// Restore user scale
			dc.SetUserScale(userScale.m_x, userScale.m_y);
		}
	}
	wxSize HTMLRenderer::GetCellSize() const
	{
		if (m_Value.HasText())
		{
			return GetRenderEngine().GetMultilineTextExtent(m_Value.GetText());
		}
		return wxSize(0, 0);
	}
}
