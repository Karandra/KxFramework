#include "stdafx.h"
#include "AuiNotebook.h"
#include "Kx/Drawing/UxTheme.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(AuiNotebook, wxAuiNotebook);

	bool AuiNotebook::Create(wxWindow* parent,
							 wxWindowID id,
							 AuiNotebookStyle style
	)
	{
		if (wxAuiNotebook::Create(parent, id, wxDefaultPosition, wxDefaultSize, ToInt(style)))
		{
			wxFont font = parent->GetFont();
			SetNormalFont(font);
			SetMeasuringFont(font);
			SetSelectedFont(font);

			m_mgr.SetArtProvider(new AuiNotebookDockArtProvider(*this));
			SetRenderer(RendererType::Default);
			return true;
		}
		return false;
	}

	void AuiNotebook::SetRenderer(RendererType value)
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
				SetArtProvider(new AuiDefaultTabArt(*this));
			}
		};
	}
}

namespace KxFramework::UI
{
	void AuiDefaultTabArt::DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect)
	{
		AuiDefaultTabArtBase::DrawBackground(dc, window, rect);
	}
	void AuiDefaultTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
	{
		AuiDefaultTabArtBase::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
	}
	void AuiDefaultTabArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect)
	{
		AuiDefaultTabArtBase::DrawButton(dc, wnd, inRect, bitmapId, buttonState, orientation, outRect);
	}
}

namespace KxFramework::UI
{
	void AuiNotebookDockArtProvider::DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect)
	{
		AuiDefaultDockArtBase::DrawBackground(dc, window, orientation, rect);
	}
	void AuiNotebookDockArtProvider::DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane)
	{
		AuiDefaultDockArtBase::DrawBorder(dc, window, rect, pane);

		// Top horizontal line
		wxRect topLine(rect.GetLeftTop(), rect.GetRightTop());
		topLine.height = 1;
		topLine.width += 1;
		UxTheme::DrawParentBackground(*m_Notebook, dc, topLine);

		// Right vertical line
		wxRect rightLine = wxRect(topLine.GetRightTop(), topLine.GetRightBottom());
		rightLine.x += 1;
		rightLine.y -= 2;
		rightLine.width += 5;
		rightLine.height += 2;
		UxTheme::DrawParentBackground(*m_Notebook, dc, rightLine);

		// Left vertical line
		wxRect leftLine;
		leftLine.x = 0;
		leftLine.y = 0;
		leftLine.width = m_Notebook->GetTabIndent();
		leftLine.height = m_Notebook->GetTabCtrlHeight() - 1;
		UxTheme::DrawParentBackground(*m_Notebook, dc, leftLine);
	}
}
