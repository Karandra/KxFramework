#include "stdafx.h"
#include "KxFramework/KxHTMLWindow.h"
#include "KxFramework/KxMenu.h"
#include "Kx/General/StdID.h"
#include "Kx/Localization/Common.h"
#include <wx/clipbrd.h>

using namespace KxFramework;

wxIMPLEMENT_DYNAMIC_CLASS(KxHTMLWindow, wxHtmlWindow);

wxString KxHTMLWindow::ProcessPlainText(const wxString& text)
{
	if (text.StartsWith(wxS("<html>")) && text.EndsWith(wxS("</html>")))
	{
		return text;
	}
	else
	{
		wxString temp = text;
		temp.Replace(wxS("\r\n"), wxS("<br/>"));
		temp.Replace(wxS("\r"), wxS("<br/>"));
		temp.Replace(wxS("\n"), wxS("<br/>"));
		return temp;
	}
}
bool KxHTMLWindow::SetupFontsUsing(const wxFont& normalFont, wxString& normalFace, wxString& fixedFace, int& pointSize)
{
	if (normalFont.IsOk())
	{
		wxFont fixedFont(normalFont);
		auto UseFixedFont = [&]()
		{
			pointSize = normalFont.GetPointSize();
			normalFace = normalFont.GetFaceName();
			fixedFace = fixedFont.GetFaceName();
		};

		if (fixedFont.SetFaceName(wxS("Consolas")) || fixedFont.SetFaceName(wxS("Courier New")))
		{
			UseFixedFont();
		}
		else if (fixedFont.SetFamily(wxFONTFAMILY_TELETYPE); fixedFont.IsOk())
		{
			UseFixedFont();
		}
		else
		{
			pointSize = normalFont.GetPointSize();
			normalFace = normalFont.GetFaceName();
			fixedFace = normalFont.GetFaceName();
		}
		return true;
	}
	return false;
}

void KxHTMLWindow::CopyTextToClipboard(const wxString& value) const
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(value));
		wxTheClipboard->Close();
	}
}
void KxHTMLWindow::CreateContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link)
{
	auto MakeItem = [&menu](int id)
	{
		return menu.AddItem(id, Localization::GetStandardLocalizedString(id));
	};

	{
		KxMenuItem* item = MakeItem(wxID_UNDO);
		item->Enable(CanUndo());
	}
	{
		KxMenuItem* item = MakeItem(wxID_REDO);
		item->Enable(CanRedo());
	}
	menu.AddSeparator();

	{
		KxMenuItem* item = MakeItem(wxID_CUT);
		item->Enable(CanCut());
	}
	{
		KxMenuItem* item = MakeItem(wxID_COPY);
		item->Enable(CanCopy());
	}
	if (link != nullptr)
	{
		KxMenuItem* item = MakeItem(ToInt(StdID::CopyLink));
	}
	{
		KxMenuItem* item = MakeItem(wxID_PASTE);
		item->Enable(CanPaste());
	}
	{
		KxMenuItem* item = MakeItem(wxID_DELETE);
		item->Enable(IsEditable());
	}
	menu.AddSeparator();
	{
		KxMenuItem* item = MakeItem(wxID_SELECTALL);
		item->Enable(!IsEmpty());
	}
}
void KxHTMLWindow::ExecuteContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link)
{
	switch (menu.Show(this))
	{
		case wxID_COPY:
		{
			Copy();
			break;
		}
		case ToInt(StdID::CopyLink):
		{
			CopyTextToClipboard(link->GetHref());
			break;
		}
		case wxID_SELECTALL:
		{
			SelectAll();
			break;
		}
	};
}

void KxHTMLWindow::OnContextMenu(wxContextMenuEvent& event)
{
	KxMenu menu;
	CreateContextMenu(menu);
	ExecuteContextMenu(menu);

	event.Skip();
}
void KxHTMLWindow::OnKey(wxKeyEvent& event)
{
	if (event.ControlDown())
	{
		switch (event.GetKeyCode())
		{
			case 'A':
			{
				SelectAll();
				break;
			}
			case 'C':
			{
				Copy();
				break;
			}
		};
	}
	event.Skip();
}

wxString KxHTMLWindow::OnProcessPlainText(const wxString& text) const
{
	return ProcessPlainText(text);
}
void KxHTMLWindow::OnHTMLLinkClicked(const wxHtmlLinkInfo& link)
{
	const wxMouseEvent* event = link.GetEvent();
	if (event && event->RightUp())
	{
		KxMenu menu;
		CreateContextMenu(menu, &link);
		ExecuteContextMenu(menu, &link);
	}
	else
	{
		wxHtmlWindow::OnHTMLLinkClicked(link);
	}
}
wxHtmlOpeningStatus KxHTMLWindow::OnHTMLOpeningURL(wxHtmlURLType type, const wxString& url, wxString* redirect) const
{
	return wxHtmlWindow::OnHTMLOpeningURL(type, url, redirect);
}

bool KxHTMLWindow::DoSetFont(const wxFont& normalFont)
{
	int pointSize = normalFont.GetPointSize();
	wxString normalFace;
	wxString fixedFace;

	if (SetupFontsUsing(normalFont, normalFace, fixedFace, pointSize))
	{
		wxHtmlWindow::SetStandardFonts(pointSize, normalFace, fixedFace);
		return true;
	}
	return false;
}
bool KxHTMLWindow::DoSetValue(const wxString& value)
{
	return wxHtmlWindow::SetPage(OnProcessPlainText(value));
}
bool KxHTMLWindow::DoAppendValue(const wxString& value)
{
	return wxHtmlWindow::AppendToPage(OnProcessPlainText(value));
}

void KxHTMLWindow::OnEraseBackground(wxEraseEvent& event)
{
	// Taken from 'wxHtmlWindow::DoEraseBackground'
	wxDC& dc = *event.GetDC();
	
	dc.SetBackground(m_BackgroundColor);
	dc.Clear();

	if (m_BackgroundBitmap.IsOk())
	{
		// Draw the background bitmap tiling it over the entire window area.
		const wxSize virtualSize = GetVirtualSize();
		const wxSize bitmapSize = m_BackgroundBitmap.GetSize();
		for (wxCoord x = 0; x < virtualSize.x; x += bitmapSize.x)
		{
			for (wxCoord y = 0; y < virtualSize.y; y += bitmapSize.y)
			{
				dc.DrawBitmap(m_BackgroundBitmap, x, y);
			}
		}
	}
}
void KxHTMLWindow::OnPaint(wxPaintEvent& event)
{
	// Taken from 'wxHtmlWindow::OnPaint'
	wxAutoBufferedPaintDC paintDC(this);
	if (IsFrozen() || !m_Cell)
	{
		return;
	}

	int x, y;
	GetViewStart(&x, &y);
	const wxRect rect = GetUpdateRegion().GetBox();

	// Don't bother drawing the empty window.
	const wxSize clientSize = GetClientSize();
	if (clientSize.x == 0 || clientSize.y == 0)
	{
		return;
	}

	auto Draw = [this, &rect, x, y](wxDC& dc)
	{
		wxEraseEvent eraseEvent(GetId(), &dc);
		OnEraseBackground(eraseEvent);

		// Draw the HTML window contents
		dc.SetMapMode(wxMM_TEXT);
		dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
		dc.SetLayoutDirection(GetLayoutDirection());

		wxHtmlRenderingInfo renderInfo;
		wxDefaultHtmlRenderingStyle renderStyle;
		renderInfo.SetSelection(m_selection);
		renderInfo.SetStyle(&renderStyle);
		m_Cell->Draw(dc, 0, 0, y * wxHTML_SCROLL_STEP + rect.GetTop(), y * wxHTML_SCROLL_STEP + rect.GetBottom(), renderInfo);
	};

	PrepareDC(paintDC);
	if (m_Renderer)
	{
		wxGCDC dc(m_Renderer->CreateContext(paintDC));
		Draw(dc);
	}
	else
	{
		Draw(paintDC);
	}
}

bool KxHTMLWindow::Create(wxWindow* parent,
						   wxWindowID id,
						   const wxString& text,
						   long style
)
{
	if (wxHtmlWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize, style, "htmlWindow"))
	{
		m_BackgroundColor = wxHtmlWindow::GetBackgroundColour();
		SetBorders(2);
		DoSetFont(parent->GetFont());
		DoSetValue(text);

		SetBackgroundStyle(wxBG_STYLE_PAINT);
		Bind(wxEVT_PAINT, &KxHTMLWindow::OnPaint, this);
		Bind(wxEVT_ERASE_BACKGROUND, &KxHTMLWindow::OnEraseBackground, this);

		Bind(wxEVT_CONTEXT_MENU, &KxHTMLWindow::OnContextMenu, this);
		Bind(wxEVT_KEY_DOWN, &KxHTMLWindow::OnKey, this);
		return true;
	}
	return false;
}
