#include "KxStdAfx.h"
#include "KxFramework/KxHTMLWindow.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"
#include <wx/clipbrd.h>

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
	auto MakeItem = [&menu](KxStandardID id)
	{
		return menu.AddItem(id, KxTranslation::GetCurrent().GetString(id));
	};

	{
		KxMenuItem* item = MakeItem(KxID_UNDO);
		item->Enable(CanUndo());
	}
	{
		KxMenuItem* item = MakeItem(KxID_REDO);
		item->Enable(CanRedo());
	}
	menu.AddSeparator();

	{
		KxMenuItem* item = MakeItem(KxID_CUT);
		item->Enable(CanCut());
	}
	{
		KxMenuItem* item = MakeItem(KxID_COPY);
		item->Enable(CanCopy());
	}
	if (link != nullptr)
	{
		KxMenuItem* item = MakeItem(KxID_COPY_LINK);
	}
	{
		KxMenuItem* item = MakeItem(KxID_PASTE);
		item->Enable(CanPaste());
	}
	{
		KxMenuItem* item = MakeItem(KxID_DELETE);
		item->Enable(IsEditable());
	}
	menu.AddSeparator();
	{
		KxMenuItem* item = MakeItem(KxID_SELECTALL);
		item->Enable(!IsEmpty());
	}
}
void KxHTMLWindow::ExecuteContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link)
{
	switch (menu.Show(this))
	{
		case KxID_COPY:
		{
			Copy();
			break;
		}
		case KxID_COPY_LINK:
		{
			CopyTextToClipboard(link->GetHref());
			break;
		}
		case KxID_SELECTALL:
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
	wxFont fixedFont(normalFont);
	auto UseFixedFont = [this, &normalFont, &fixedFont]()
	{
		wxHtmlWindow::SetStandardFonts(normalFont.GetPointSize(), normalFont.GetFaceName(), fixedFont.GetFaceName());
	};

	if (fixedFont.SetFaceName(wxS("Consolas")))
	{
		UseFixedFont();
	}
	else if (fixedFont.SetFaceName(wxS("Courier New")))
	{
		UseFixedFont();
	}
	else if (fixedFont.SetFamily(wxFONTFAMILY_TELETYPE); fixedFont.IsOk())
	{
		UseFixedFont();
	}
	else if (normalFont.IsOk())
	{
		wxHtmlWindow::SetStandardFonts(normalFont.GetPointSize(), normalFont.GetFaceName(), normalFont.GetFaceName());
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

bool KxHTMLWindow::Create(wxWindow* parent,
						   wxWindowID id,
						   const wxString& text,
						   long style
)
{
	if (wxHtmlWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize, style, "htmlWindow"))
	{
		SetBorders(2);
		DoSetFont(parent->GetFont());
		DoSetValue(text);

		Bind(wxEVT_CONTEXT_MENU, &KxHTMLWindow::OnContextMenu, this);
		Bind(wxEVT_KEY_DOWN, &KxHTMLWindow::OnKey, this);
		return true;
	}
	return false;
}
KxHTMLWindow::~KxHTMLWindow()
{
}
