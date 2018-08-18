#include "KxStdAfx.h"
#include "KxFramework/KxSplitterWindow.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxSplitterWindow, wxSplitterWindow);

void KxSplitterWindow::OnDoubleClickSash(int x, int y)
{
	SetSashPosition(m_InitialPosition != 0 ? m_InitialPosition : GetMinimumPaneSize(), true);
}
void KxSplitterWindow::OnDrawSash(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);

	int sashSize = GetSashSize();
	if (sashSize != 0)
	{
		dc.SetBrush(m_SashColor);
		dc.SetPen(*wxTRANSPARENT_PEN);

		if (GetSplitMode() == wxSPLIT_VERTICAL)
		{
			dc.DrawRectangle(GetSashPosition(), 0, sashSize, GetSize().GetHeight());
		}
		else
		{
			dc.DrawRectangle(0, GetSashPosition(), GetSize().GetWidth(), sashSize);
		}
	}
}

bool KxSplitterWindow::Create(wxWindow* parent,
							  wxWindowID id,
							  long style
)
{
	if (wxSplitterWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize, style))
	{
		wxSplitterWindow::m_isHot = true;
		return true;
	}
	return false;
}
KxSplitterWindow::~KxSplitterWindow()
{
}

bool KxSplitterWindow::SplitHorizontally(wxWindow* window1, wxWindow* window2, int sashPosition)
{
	m_InitialPosition = sashPosition;
	return wxSplitterWindow::SplitHorizontally(window1, window2, sashPosition);
}
bool KxSplitterWindow::SplitVertically(wxWindow* window1, wxWindow* window2, int sashPosition)
{
	m_InitialPosition = sashPosition;
	return wxSplitterWindow::SplitVertically(window1, window2, sashPosition);
}

void KxSplitterWindow::SetInitialPosition(int pos)
{
	m_InitialPosition = pos;
	SetSashPosition(pos, false);
	//wxSplitterWindow::m_sashPosition = pos;
	//wxSplitterWindow::m_sashPositionCurrent = pos;
	//wxSplitterWindow::m_needUpdating = true;
}

void KxSplitterWindow::SetSashColor(const wxColour& color)
{
	m_SashColor = color;
	if (m_SashColor.IsOk())
	{
		SetBackgroundStyle(wxBG_STYLE_PAINT);
		Bind(wxEVT_PAINT, &KxSplitterWindow::OnDrawSash, this);
	}
	else
	{
		SetBackgroundStyle(wxBG_STYLE_ERASE);
		Unbind(wxEVT_PAINT, &KxSplitterWindow::OnDrawSash, this);
	}
}
