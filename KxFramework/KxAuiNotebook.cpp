#include "KxStdAfx.h"
#include "KxFramework/KxAuiNotebook.h"
#include "KxFramework/KxPanel.h"
#include "KxFramework/KxUxTheme.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxAuiNotebook, wxAuiNotebook);

bool KxAuiNotebook::Create(wxWindow* parent,
						   wxWindowID id,
						   long style
)
{
	if (wxAuiNotebook::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		wxFont font = parent->GetFont();
		SetNormalFont(font);
		SetMeasuringFont(font);
		SetSelectedFont(font);

		m_mgr.SetArtProvider(new KxAuiNotebookDockArtProvider(this));
		SetRenderer(Default);
		return true;
	}
	return false;
}

void KxAuiNotebook::SetRenderer(RendererType value)
{
	m_CurrentRendererType = value;
	switch (value)
	{
		case RendererType::Generic:
		{
			SetArtProvider(new wxAuiGenericTabArt());
			break;
		}
		case RendererType::Simple:
		{
			SetArtProvider(new wxAuiSimpleTabArt());
			break;
		}
		default:
		{
			SetArtProvider(new KxAuiDefaultTabArt(this));
		}
	};
}

//////////////////////////////////////////////////////////////////////////
KxAuiDefaultTabArt::KxAuiDefaultTabArt(KxAuiNotebook* object)
	:m_Notebook(object)
{
}

void KxAuiDefaultTabArt::DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect)
{
	KxAuiDefaultTabArtBase::DrawBackground(dc, window, rect);
}
void KxAuiDefaultTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
{
	KxAuiDefaultTabArtBase::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
}
void KxAuiDefaultTabArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect)
{
	KxAuiDefaultTabArtBase::DrawButton(dc, wnd, inRect, bitmapId, buttonState, orientation, outRect);
}

//////////////////////////////////////////////////////////////////////////
KxAuiNotebookDockArtProvider::KxAuiNotebookDockArtProvider(KxAuiNotebook* object)
	:m_Notebook(object)
{
	//SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	//SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, wxColour("RED"));
	//SetColor(wxAUI_DOCKART_BORDER_COLOUR, wxColour("BLUE"));
}

void KxAuiNotebookDockArtProvider::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
{
	KxAuiDefaultDockArtBase::DrawBackground(dc, window, orientation, rect);
}
void KxAuiNotebookDockArtProvider::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
{
	KxAuiDefaultDockArtBase::DrawBorder(dc, window, rect, pane);

	// Top horizontal line
	wxRect topLine(rect.GetLeftTop(), rect.GetRightTop());
	topLine.height = 1;
	topLine.width += 1;
	KxUxTheme::DrawParentBackground(*m_Notebook, dc, topLine);

	// Right vertical line
	wxRect rightLine = wxRect(topLine.GetRightTop(), topLine.GetRightBottom());
	rightLine.x += 1;
	rightLine.y -= 2;
	rightLine.width += 5;
	rightLine.height += 2;
	KxUxTheme::DrawParentBackground(*m_Notebook, dc, rightLine);

	// Left vertical line
	wxRect leftLine;
	leftLine.x = 0;
	leftLine.y = 0;
	leftLine.width = m_Notebook->GetTabIndent();
	leftLine.height = m_Notebook->GetTabCtrlHeight() - 1;
	KxUxTheme::DrawParentBackground(*m_Notebook, dc, leftLine);
}
