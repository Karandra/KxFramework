#include "KxStdAfx.h"
#include "KxFramework/KxTextBox.h"
#include <CommCtrl.h>
#include "Kx/System/UndefWindows.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxTextBox, wxTextCtrl)

const wxString KxTextBox::DefaultPasswordChar = wxUniChar(0x25CF); // Bold dot 

bool KxTextBox::Create(wxWindow* parent,
					   wxWindowID id,
					   const wxString& value,
					   const wxPoint& pos,
					   const wxSize& size,
					   long style,
					   const wxValidator& validator
)
{
	if (wxTextCtrl::Create(parent, id, value, pos, size, style, validator))
	{
		SetDoubleBuffered(true);
		SetTabWidth();

		return false;
	}
	return true;
}

WXLRESULT KxTextBox::MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
{
	// Ctrl + A (A is 1)
	if (msg == WM_CHAR && wParam == 1)
	{
		::SendMessageW(GetHandle(), EM_SETSEL, 0, -1);
		return 1;
	}
	return wxTextCtrl::MSWWindowProc(msg, wParam, lParam);
}

bool KxTextBox::SetTabWidth(unsigned int width)
{
	const bool ret = ::SendMessageW(GetHandle(), EM_SETTABSTOPS, 1, (LPARAM)&width) != 0;
	Refresh();
	return ret;
}
void KxTextBox::SetLimit(size_t max)
{
	::SendMessageW(GetHandle(), EM_LIMITTEXT, max, 0);
}

void KxTextBox::SetPasswordChar(const wxString& character)
{
	wxUint32 charValue = 0;
	if (!character.IsEmpty())
	{
		charValue = character.GetChar(0).GetValue();
	}
	::SendMessageW(GetHandle(), EM_SETPASSWORDCHAR, charValue, 0);
	Refresh();
}
bool KxTextBox::SetHint(const wxString& label)
{
	wxTextCtrl::SetHint(label);
	return ::SendMessageW(GetHandle(), EM_SETCUEBANNER, TRUE, (LPARAM)label.wc_str());
}

void KxTextBox::SetValue(const wxString& value)
{
	wxTextCtrl::ChangeValue(value);
}
void KxTextBox::SetValueEvent(const wxString& value)
{
	wxTextCtrl::SetValue(value);
}

void KxTextBox::Clear()
{
	wxEventBlocker block(this, wxEVT_TEXT);
	wxTextCtrl::Clear();
}
void KxTextBox::ClearEvent()
{
	wxTextCtrl::Clear();
}
