#include "KxStdAfx.h"
#include "KxValidator.h"
#include <wx/spinctrl.h>

namespace
{
	class SpinCtrlDoubleWrapper: public wxSpinCtrlDouble
	{
		public:
			wxString DoValueToText(double value) override
			{
				return wxSpinCtrlDouble::DoValueToText(value);
			}
	};

	void GetTextSelection(HWND textArea, int& selectionStart, int& selectionEnd)
	{
		DWORD start = 0;
		DWORD end = 0;
		::SendMessageW(textArea, EM_GETSEL, reinterpret_cast<WPARAM>(&start), reinterpret_cast<LPARAM>(&end));

		selectionStart = start;
		selectionEnd = end;
	}
	void ProcessTextSelection(wxString& value, int& position, int selectionStart, int selectionEnd)
	{
		if (size_t selectionLength = selectionEnd - selectionStart; selectionLength != 0)
		{
			// Remove selected text because pressing a key would make it disappear.
			value.erase(selectionStart, selectionLength);

			// And adjust the insertion point to have correct position in the new string.
			if (position > selectionStart)
			{
				if (position >= selectionEnd)
				{
					position -= selectionLength;
				}
				else
				{
					position = selectionStart;
				}
			}
		}
	}
}

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

		// If no appropriate control will be found, ensure that uninitialized int don't cause any troubles
		position = 0;

		auto ProcessSelectionStartEnd = [](HWND textArea, const wxString& value, int& position)
		{
			int selectionStart = 0;
			int selectionEnd = 0;
			GetTextSelection(textArea, selectionStart, selectionEnd);

			if (selectionEnd == 0 && !value.IsEmpty())
			{
				selectionEnd = value.size();
			}

			position = selectionStart;
			return std::tuple{selectionStart, selectionEnd};
		};

		if (wxTextEntry* textEntry = dynamic_cast<wxTextCtrl*>(validator.GetWindow()))
		{
			// This part replicates 'wxNumValidatorBase::GetCurrentValueAndInsertionPoint' itself

			value = textEntry->GetValue();
			position = textEntry->GetInsertionPoint();

			long selectionStart = 0;
			long selectionEnd = 0;
			textEntry->GetSelection(&selectionStart, &selectionEnd);

			ProcessTextSelection(value, position, selectionStart, selectionEnd);
		}
		else if (wxSpinCtrl* spin = dynamic_cast<wxSpinCtrl*>(validator.GetWindow()))
		{
			HWND textArea = ::GetWindow(spin->GetHandle(), GW_HWNDPREV);
			if (textArea)
			{
				value = KxFormat(wxS("%1"))(spin->GetValue(), 0, spin->GetBase());

				auto [selectionStart, selectionEnd] = ProcessSelectionStartEnd(textArea, value, position);
				ProcessTextSelection(value, position, selectionStart, selectionEnd);
			}
		}
		else if (wxSpinCtrlDouble* spinDouble = dynamic_cast<wxSpinCtrlDouble*>(validator.GetWindow()))
		{
			HWND textArea = ::GetWindow(spinDouble->GetHandle(), GW_HWNDPREV);
			if (textArea)
			{
				SpinCtrlDoubleWrapper* wrapper = reinterpret_cast<SpinCtrlDoubleWrapper*>(spinDouble);
				value = wrapper->DoValueToText(spinDouble->GetValue());

				auto [selectionStart, selectionEnd] = ProcessSelectionStartEnd(textArea, value, position);
				ProcessTextSelection(value, position, selectionStart, selectionEnd);
			}
		}

		// Clamp position just in case something gone wrong
		position = std::clamp<int>(position, 0, value.size());
	}
}
