#include "KxStdAfx.h"
#include "KxFramework/KxHTMLWindow.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxMenu.h"
#include "KxFramework/KxUtility.h"
#include <wx/clipbrd.h>

wxIMPLEMENT_DYNAMIC_CLASS(KxHTMLWindow, wxHtmlWindow);

wxString KxHTMLWindow::ProcessPlainText(const wxString& text)
{
	if (text.StartsWith("<html>") && text.EndsWith("</html>"))
	{
		return text;
	}
	else
	{
		wxString temp = text;
		temp.Replace("\r\n", "<br>");
		temp.Replace("\r", "<br>");
		temp.Replace("\n", "<br>");
		return temp;
	}
}

void KxHTMLWindow::DoSetFont(const wxFont& font)
{
	SetStandardFonts(font.GetPointSize(), font.GetFaceName(), font.GetFaceName());
}
void KxHTMLWindow::CreateContextMenu()
{
	m_ContextMenu.Bind(KxEVT_MENU_OPEN, &KxHTMLWindow::OnShowContextMenu, this);

	m_ContextMenu.Add(new KxMenuItem(wxID_UNDO, KxTranslation::GetCurrent().GetString(wxID_UNDO)));
	m_ContextMenu.AddSeparator();

	m_ContextMenu.Add(new KxMenuItem(wxID_CUT, KxTranslation::GetCurrent().GetString(wxID_CUT)));
	m_ContextMenu.Add(new KxMenuItem(wxID_COPY, KxTranslation::GetCurrent().GetString(wxID_COPY)));
	m_ContextMenu.Add(new KxMenuItem(wxID_PASTE, KxTranslation::GetCurrent().GetString(wxID_PASTE)));
	m_ContextMenu.Add(new KxMenuItem(wxID_REMOVE, KxTranslation::GetCurrent().GetString(wxID_REMOVE)));
	m_ContextMenu.AddSeparator();

	m_ContextMenu.Add(new KxMenuItem(wxID_SELECTALL, KxTranslation::GetCurrent().GetString(wxID_SELECTALL)));
}
void KxHTMLWindow::CopyTextToClipboard(const wxString& value) const
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(value));
		wxTheClipboard->Close();
	}
}

void KxHTMLWindow::OnShowContextMenu(KxMenuEvent& event)
{
	for (wxMenuItem* item: m_ContextMenu.GetMenuItems())
	{
		switch (static_cast<KxMenuItem*>(item)->GetId())
		{
			case wxID_COPY:
			case wxID_SELECTALL:
			{
				item->Enable(!IsEmpty());
				break;
			}
			case wxID_UNDO:
			case wxID_CUT:
			case wxID_PASTE:
			case wxID_REMOVE:
			{
				item->Enable(IsEditable());
				break;
			}
		};
	}
}
void KxHTMLWindow::OnContextMenu(wxContextMenuEvent& event)
{
	wxWindowID id = m_ContextMenu.Show(this);
	switch (id)
	{
		case wxID_COPY:
		{
			CopyTextToClipboard(SelectionToText());
			break;
		}
		case wxID_SELECTALL:
		{
			SelectAll();
			break;
		}
	};
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
		CreateContextMenu();

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
	m_Value = value;
	return wxHtmlWindow::SetPage(value);
}
void KxHTMLWindow::Clear()
{
	m_Value.Clear();
	wxHtmlWindow::SetPage(m_Value);
}
bool KxHTMLWindow::SetTextValue(const wxString& text)
{
	return SetValue(ProcessPlainText(text));
}

KxHTMLWindow& KxHTMLWindow::operator<<(const wxString& s)
{
	SetValue(m_Value + ProcessPlainText(s));
	return *this;
}
