#include "KxStdAfx.h"
#include "KxFramework/KxHTMLWindow.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"
#include <wx/clipbrd.h>

namespace
{
	void CopyTextToClipboard(const wxString& value)
	{
		if (wxTheClipboard->Open())
		{
			wxTheClipboard->SetData(new wxTextDataObject(value));
			wxTheClipboard->Close();
		}
	}
}

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
		temp.Replace(wxS("\r\n"), wxS("<br>"));
		temp.Replace(wxS("\r"), wxS("<br>"));
		temp.Replace(wxS("\n"), wxS("<br>"));
		return temp;
	}
}

void KxHTMLWindow::CreateContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link)
{
	auto MakeItem = [&menu](KxStandardID id)
	{
		return menu.Add(new KxMenuItem(id, KxTranslation::GetCurrent().GetString(id)));
	};

	{
		KxMenuItem* item = MakeItem(KxID_UNDO);
		item->Enable(IsEditable());
	}
	{
		KxMenuItem* item = MakeItem(KxID_REDO);
		item->Enable(IsEditable());
	}
	menu.AddSeparator();

	{
		KxMenuItem* item = MakeItem(KxID_CUT);
		item->Enable(IsEditable());
	}
	{
		KxMenuItem* item = MakeItem(KxID_COPY);
		item->Enable(HasSelection());
	}
	if (link != NULL)
	{
		KxMenuItem* item = MakeItem(KxID_COPY_LINK);
	}
	{
		KxMenuItem* item = MakeItem(KxID_PASTE);
		item->Enable(IsEditable());
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
wxWindowID KxHTMLWindow::ExecuteContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link)
{
	wxWindowID id = menu.Show(this);
	switch (id)
	{
		case KxID_COPY:
		{
			CopyTextToClipboard(SelectionToText());
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
	return id;
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
				CopyTextToClipboard(SelectionToText());
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

void KxHTMLWindow::DoSetFont(const wxFont& font)
{
	SetStandardFonts(font.GetPointSize(), font.GetFaceName(), font.GetFaceName());
}
bool KxHTMLWindow::DoSetValue(const wxString& value)
{
	m_Value = value;
	return wxHtmlWindow::SetPage(value);
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

const wxString& KxHTMLWindow::GetValue() const
{
	return m_Value;
}
bool KxHTMLWindow::SetValue(const wxString& value)
{
	return DoSetValue(value);
}
bool KxHTMLWindow::SetTextValue(const wxString& text)
{
	return DoSetValue(OnProcessPlainText(text));
}

void KxHTMLWindow::Clear()
{
	m_Value.Clear();
	wxHtmlWindow::SetPage(m_Value);
}
bool KxHTMLWindow::IsEmpty() const
{
	return m_Value.IsEmpty();
}

bool KxHTMLWindow::IsEditable() const
{
	return m_IsEditable;
}
void KxHTMLWindow::SetEditable(bool isEditable)
{
	// Not implemented
	//m_IsEditable = bIsEditable;
}

bool KxHTMLWindow::HasSelection() const
{
	return m_selection && !m_selection->IsEmpty() && !IsEmpty();
}

KxHTMLWindow& KxHTMLWindow::operator<<(const wxString& s)
{
	SetValue(m_Value + OnProcessPlainText(s));
	return *this;
}
