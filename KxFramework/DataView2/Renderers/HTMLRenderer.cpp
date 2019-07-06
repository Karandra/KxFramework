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
		m_Content.clear();
		m_ContentHTML.clear();

		if (value.GetAs(&m_Content))
		{
			m_ContentHTML = KxHTMLWindow::ProcessPlainText(m_Content);
			return true;
		}
		else
		{
			return false;
		}
	}
	ToolTip HTMLRenderer::CreateToolTip() const
	{
		return ToolTip::CreateDefaultForRenderer(m_Content);
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
		if (!m_Content.IsEmpty())
		{
			// Prefer regular DC
			wxDC& dc = HasRegularDC() ? GetRegularDC() : GetGraphicsDC();
			wxHtmlDCRenderer htmlRenderer;
			PrepareRenderer(htmlRenderer, dc, cellRect);

			// Save user scale
			wxPoint2DDouble userScale(1.0,  1.0);
			dc.GetUserScale(&userScale.m_x, &userScale.m_y);

			// Render text
			wxDCClipper clip(dc, cellRect);
			htmlRenderer.Render(cellRect.GetX(), cellRect.GetY(), m_VisibleCellFrom, m_VisibleCellTo);

			// Restore user scale
			dc.SetUserScale(userScale.m_x, userScale.m_y);
		}
	}
	wxSize HTMLRenderer::GetCellSize() const
	{
		// HTMLRenderer is the only renderer at the moment that supports multiline text.
		// RenderEngine doesn't have a function to measure multiline text (it measures only the first line).
		// So we need to do that ourselves.

		if (!m_Content.IsEmpty())
		{
			if (HasRegularDC())
			{
				return GetRegularDC().GetMultiLineTextExtent(m_Content);
			}
			else if (HasGraphicsDC())
			{
				return GetGraphicsDC().GetMultiLineTextExtent(m_Content);
			}
			return wxClientDC(GetView()).GetMultiLineTextExtent(m_Content);
		}
		return wxSize(0, 0);
	}
}
