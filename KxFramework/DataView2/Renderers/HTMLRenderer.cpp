#include "KxStdAfx.h"
#include "HTMLRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"
#include "KxFramework/KxHTMLWindow.h"
#include <wx/html/htmprint.h>

namespace
{
	// For some unknown reason scale of '1.0' renders text too large.
	// Scaling it to '0.8' solves this.
	constexpr double g_UserScale = 0.8;

	class DCUserScaleSaver final
	{
		private:
			wxDC& m_DC;
			wxPoint2DDouble m_UserScale;

		public:
			DCUserScaleSaver(wxDC& dc)
				:m_DC(dc)
			{
				m_DC.GetUserScale(&m_UserScale.m_x, &m_UserScale.m_y);
			}
			~DCUserScaleSaver()
			{
				m_DC.SetUserScale(m_UserScale.m_x, m_UserScale.m_y);
			}
	};
}

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
		htmlRenderer.SetDC(&dc, g_UserScale * m_PixelScale, g_UserScale * m_FontScale);

		// Size
		wxSize size = cellRect.GetSize();
		size.IncTo(GetCellSize());
		htmlRenderer.SetSize(size.GetWidth(), size.GetHeight());

		// Set HTML
		htmlRenderer.SetHtmlText(m_ContentHTML);

		// Setup fonts. This needs to be done after a call to 'wxHtmlDCRenderer::SetHtmlText',
		// otherwise text scales really weirdly.
		int pointSize = 0;
		wxString normalFace;
		wxString fixedFace;
		if (KxHTMLWindow::SetupFontsUsing(dc.GetFont(), normalFace, fixedFace, pointSize))
		{
			htmlRenderer.SetFonts(normalFace, fixedFace);
			htmlRenderer.SetStandardFonts(pointSize, normalFace, fixedFace);
		}
	}
	void HTMLRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		if (m_Value.HasText())
		{
			// Prefer regular DC
			wxDC& dc = HasRegularDC() ? GetRegularDC() : GetGraphicsDC();
			DCUserScaleSaver userScaleSaver(dc);
			wxDCClipper clip(dc, cellRect.Deflate(0, GetRenderEngine().FromDIPY(2)));

			// Render text
			wxHtmlDCRenderer htmlRenderer;
			PrepareRenderer(htmlRenderer, dc, cellRect);
			htmlRenderer.Render(cellRect.GetX(), cellRect.GetY(), m_VisibleCellFrom, m_VisibleCellTo);
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

	HTMLRenderer::HTMLRenderer(int alignment)
		:Renderer(alignment)
	{
	}
	HTMLRenderer::~HTMLRenderer()
	{
	}
}
