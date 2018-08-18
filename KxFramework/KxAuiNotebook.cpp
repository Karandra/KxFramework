#include "KxStdAfx.h"
#include "KxFramework/KxAuiNotebook.h"
#include "KxFramework/KxPanel.h"
#include "KxFramework/KxUtility.h"

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

bool KxAuiNotebook::InsertPage(size_t index, wxWindow* window, const wxString& label, bool select, int imageID)
{
	return wxAuiNotebook::InsertPage(index, window, label, select, imageID);
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
	wxRect tTopLine(rect.GetLeftTop(), rect.GetRightTop());
	tTopLine.height = 1;
	tTopLine.width += 1;
	KxUtility::DrawParentBackground(m_Notebook, dc, tTopLine);

	// Right vertical line
	wxRect tRightLine = wxRect(tTopLine.GetRightTop(), tTopLine.GetRightBottom());
	tRightLine.x += 1;
	tRightLine.y -= 2;
	tRightLine.width =+ 5;
	tRightLine.height += 2;
	KxUtility::DrawParentBackground(m_Notebook, dc, tRightLine);

	// Left vertical line
	wxRect tLeftLine;
	tLeftLine.x = 0;
	tLeftLine.y = 0;
	tLeftLine.width = m_Notebook->GetTabIndent();
	tLeftLine.height = m_Notebook->GetTabCtrlHeight() - 1;
	KxUtility::DrawParentBackground(m_Notebook, dc, tLeftLine);
}
