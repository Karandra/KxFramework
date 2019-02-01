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

	void HTMLRenderer::PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, wxDC& dc, const wxRect& cellRect) const
	{
		// For some unknown reason scale of '1.0' renders text too large.
		// Scaling it to '0.8' solves this.
		const double userScale = 0.8;
		htmlRenderer.SetDC(&dc, userScale * m_PixelScale, userScale * m_FontScale);

		// Size
		wxSize size = cellRect.GetSize();
		size.IncTo(GetCellSize());
		htmlRenderer.SetSize(size.GetWidth(), size.GetHeight());

		// Content
		htmlRenderer.SetHtmlText(m_ContentHTML);

		const wxFont& font = dc.GetFont();
		htmlRenderer.SetFonts(font.GetFaceName(), "Consolas");
		htmlRenderer.SetStandardFonts(font.GetPointSize(), font.GetFaceName(), "Consolas");
	}
	void HTMLRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		if (!m_Content.IsEmpty())
		{
			wxDC& dc = GetGraphicsDC();
			wxHtmlDCRenderer htmlRenderer;
			PrepareRenderer(htmlRenderer, dc, cellRect);

			// Save user scale
			double scaleX = 1.0;
			double scaleY = 1.0;
			dc.GetUserScale(&scaleX, &scaleY);

			// Render text
			htmlRenderer.Render(cellRect.GetX(), cellRect.GetY(), m_VisibleCellFrom, m_VisibleCellTo);

			// Restore user scale
			dc.SetUserScale(scaleX, scaleY);
		}
	}
	wxSize HTMLRenderer::GetCellSize() const
	{
		if (!m_Content.IsEmpty())
		{
			if (HasGraphicsDC())
			{
				return GetGraphicsDC().GetMultiLineTextExtent(m_Content);
			}
			else if (HasRegularDC())
			{
				return GetRegularDC().GetMultiLineTextExtent(m_Content);
			}
			return wxClientDC(GetView()).GetMultiLineTextExtent(m_Content);
		}
		return wxSize(0, 0);
	}
}
