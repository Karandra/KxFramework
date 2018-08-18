#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewHTMLRenderer.h"
#include "KxFramework/KxHTMLWindow.h"
#include <wx/html/htmprint.h>

bool KxDataViewHTMLRenderer::SetValue(const wxAny& value)
{
	m_Content.clear();
	m_ContentHTML.clear();

	if (value.GetAs<wxString>(&m_Content))
	{
		m_ContentHTML = KxHTMLWindow::ProcessPlainText(m_Content);
		return true;
	}
	else
	{
		return false;
	}
}

void KxDataViewHTMLRenderer::PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, wxDC& dc, const wxRect& cellRect) const
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
void KxDataViewHTMLRenderer::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	if (!m_Content.IsEmpty())
	{
		wxDC& dc = GetDC();

		wxHtmlDCRenderer htmlRenderer;
		PrepareRenderer(htmlRenderer, dc, cellRect);

		// Save user scale
		double scaleX = 1.0;
		double scaleY = 1.0;
		dc.GetUserScale(&scaleX, &scaleY);

		// Render text
		htmlRenderer.Render(cellRect.GetX(), cellRect.GetY(), m_KnownPageBreaks);

		// Restore user scale
		dc.SetUserScale(scaleX, scaleY);
	}
}
wxSize KxDataViewHTMLRenderer::GetCellSize() const
{
	if (!m_Content.IsEmpty())
	{
		return wxClientDC(GetView()).GetMultiLineTextExtent(m_Content);
	}
	return wxSize(0, KxDataViewRenderer::GetCellSize().GetHeight());
}

KxIntVector KxDataViewHTMLRenderer::GetKnownPageBreaks() const
{
	KxIntVector knownBreaks;
	knownBreaks.reserve(m_KnownPageBreaks.size());
	for (const auto& v: m_KnownPageBreaks)
	{
		knownBreaks.push_back(v);
	}
	return knownBreaks;
}
void KxDataViewHTMLRenderer::SetKnownPageBreaks(const KxIntVector& tBreaks)
{
	m_KnownPageBreaks.clear();
	m_KnownPageBreaks.reserve(tBreaks.size());
	for (const auto& v : tBreaks)
	{
		m_KnownPageBreaks.push_back(v);
	}
}
