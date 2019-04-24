#include "KxStdAfx.h"
#include "KxValidator.h"
#include <wx/spinctrl.h>

namespace KxValidator
{
	void FixIsCharOk(const wxValidator& validator, wxString& value, int& position, wxChar& character)
	{
		// Look in "common/valnum.cpp" at line 166.
		/*
			wxString val;
			int pos;
			GetCurrentValueAndInsertionPoint(val, pos);
		*/
		// The 'pos' (called 'position' in this function) is going to be uninitialized
		// if 'GetCurrentValueAndInsertionPoint' returns before assigning it because
		// 'wxNumValidatorBase::GetTextEntry' function can't get 'wxTextEntry' from
		// spin controls as they have no dedicated 'wxTextEntry' object (only HWND of it).

		// Pure virtual 'wxNumValidatorBase::IsCharOk' is called right after 'GetCurrentValueAndInsertionPoint'
		// without any checks, so we can do anything that function would need to do in order to get user input.

		if (wxTextEntry* textEntry = dynamic_cast<wxTextCtrl*>(validator.GetWindow()))
		{
			// This part replicates 'wxNumValidatorBase::GetCurrentValueAndInsertionPoint' itself

			value = textEntry->GetValue();
			position = textEntry->GetInsertionPoint();

			long selectionStart = 0;
			long selectionEnd = 0;
			textEntry->GetSelection(&selectionStart, &selectionEnd);
			
			if (size_t valueLength = selectionEnd - selectionStart; valueLength != 0)
			{
				// Remove selected text because pressing a key would make it disappear.
				value.erase(selectionStart, valueLength);

				// And adjust the insertion point to have correct position in the new string.
				if (position > selectionStart)
				{
					if (position >= selectionEnd)
					{
						position -= valueLength;
					}
					else
					{
						position = selectionStart;
					}
				}
			}
		}
		else if (wxSpinCtrl* spin = dynamic_cast<wxSpinCtrl*>(validator.GetWindow()))
		{
			value = wxString::Format(wxS("%d"), spin->GetValue());
			position = value.length();
		}
		else if (wxSpinCtrlDouble* spinFloat = dynamic_cast<wxSpinCtrlDouble*>(validator.GetWindow()))
		{
			value = wxString::Format(wxS("%f"), spinFloat->GetValue());
			position = value.length();
		}
		else
		{
			// If no appropriate control could be found, ensure that uninitialized int don't cause any troubles
			position = 0;
		}
	}
}
