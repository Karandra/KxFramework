#include "KxStdAfx.h"
#include "KxFramework/KxStdDialogButtonSizer.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxButton.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_CLASS(KxStdDialogButtonSizer, wxBoxSizer);

void KxStdDialogButtonSizer::ConfigureButton(wxAnyButton* button)
{
	wxString label = button->GetLabel();
	wxSize size = wxSize(button->GetTextExtent(label).GetWidth(), 21);
	size.SetWidth(size.GetWidth() + 28);
	if (size.GetWidth() < 72)
	{
		size.SetWidth(72);
	}
	button->SetMinSize(size);
	button->SetMaxSize(size);
}

KxStdDialogButtonSizer::KxStdDialogButtonSizer()
	:wxBoxSizer(wxHORIZONTAL)
{
	bool isPDA = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

	// If we have a PDA screen, put yes/no button over all other buttons, otherwise on the left side.
	if (isPDA)
	{
		SetOrientation(wxVERTICAL);
	}
}

KxButton* KxStdDialogButtonSizer::CreateButton(KxDialog* dialog, int buttons, int buttonConstant, wxWindowID id, bool addToList)
{
	if (buttons & buttonConstant)
	{
		KxButton* button = new KxButton(dialog, id);
		button->SetAllowDrawFocus(true);
		if (addToList)
		{
			AddButton(button);
		}
		return button;
	}
	return nullptr;
}
bool KxStdDialogButtonSizer::AddButton(wxAnyButton* button)
{
	bool isStandard = true;
	switch (button->GetId())
	{
		case wxID_OK:
		case wxID_YES:
		case wxID_SAVE:
		case wxID_RETRY:
		{
			m_ButtonAffirmative = button;
			break;
		}
		case wxID_APPLY:
		{
			m_ButtonApply = button;
			break;
		}
		case wxID_NO:
		{
			m_ButtonNegative = button;
			break;
		}
		case wxID_CANCEL:
		case wxID_CLOSE:
		{
			m_ButtonCancel = button;
			break;
		}
		case wxID_HELP:
		{
			m_ButtonHelp = button;
			break;
		}
		case wxID_CONTEXT_HELP:
		{
			wxWindow* window = GetContainingWindow();
			window->SetExtraStyle(window->GetExtraStyle()|wxWS_EX_CONTEXTHELP);
			break;
		}

		default:
		{
			button->SetLabel(KxUtility::GetStandardLocalizedString(button->GetId()));
			m_NonStandardButtons.push_back(button);
			isStandard = false;
		}
	};

	ConfigureButton(button);
	return isStandard;
}
void KxStdDialogButtonSizer::AddCustomButton(KxButton* button, const wxString& label, bool prepend)
{
	if (label.IsEmpty())
	{
		button->SetLabel(KxUtility::GetStandardLocalizedString(button->GetId()));
	}
	else
	{
		button->SetLabel(label);
	}
	ConfigureButton(button);

	int border = GetSpacing(button);
	if (prepend)
	{
		if (!m_HasFirstPrepend)
		{
			m_HasFirstPrepend = true;
			InsertSpacer(0, 6);
		}
		Insert(1, button, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, border);
	}
	else
	{
		Add(button, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, border);
	}
}
void KxStdDialogButtonSizer::Realize()
{
	/* Std behavior, Windows layout */
	// Right-justify buttons
	Add(0, 0, 1, wxEXPAND, 0);

	auto AddStdButton = [this](wxAnyButton* button)
	{
		if (button)
		{
			Add(button, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, button->ConvertDialogToPixels(wxSize(2, 0)).x);
		}
	};
	AddStdButton(m_ButtonAffirmative);
	AddStdButton(m_ButtonNegative);
	AddStdButton(m_ButtonCancel);
	AddStdButton(m_ButtonApply);
	AddStdButton(m_ButtonHelp);

	/* Custom layout */
	auto SetLabel = [this](wxAnyButton* button)
	{
		if (button)
		{
			button->SetLabel(KxUtility::GetStandardLocalizedString(button->GetId()));
		}
	};
	SetLabel(m_ButtonAffirmative);
	SetLabel(m_ButtonApply);
	SetLabel(m_ButtonNegative);
	SetLabel(m_ButtonCancel);
	SetLabel(m_ButtonHelp);

	for (wxAnyButton* button: m_NonStandardButtons)
	{
		Add(button, 0, wxALIGN_CENTER|wxRIGHT, GetSpacing(button));
	}
}
