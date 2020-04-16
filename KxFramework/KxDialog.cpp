#include "stdafx.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxButton.h"
#include "Kx/Localization/Common.h"

using namespace KxFramework;

wxIMPLEMENT_DYNAMIC_CLASS(KxDialog, wxDialog);

bool KxDialog::Create(wxWindow* parent,
					  wxWindowID id,
					  const wxString& title,
					  const wxPoint& pos,
					  const wxSize& size,
					  long style
)
{
	if (style & wxDIALOG_EX_CONTEXTHELP)
	{
		style = style & ~wxDIALOG_EX_CONTEXTHELP;
		SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	}

	if (wxDialog::Create(parent, id, title, pos, size, style))
	{
		SetInitialSize(size);
		SetDefaultBackgroundColor();
		CenterIfNoPosition(pos);

		return true;
	}
	return false;
}
KxDialog::~KxDialog()
{
}

UI::StdDialogButtonSizer* KxDialog::CreateStdDialogButtonSizer(StdButton buttons, StdButton defaultButtons)
{
	auto* stdButtonSizer = new UI::StdDialogButtonSizer();

	const bool addToList = true;
	KxButton* ok = stdButtonSizer->CreateButton(this, buttons, StdButton::OK, wxID_OK, addToList);
	KxButton* yes = stdButtonSizer->CreateButton(this, buttons, StdButton::Yes, wxID_YES, addToList);
	KxButton* no = stdButtonSizer->CreateButton(this, buttons, StdButton::No, wxID_NO, addToList);
	stdButtonSizer->CreateButton(this, buttons, StdButton::Retry, wxID_RETRY, addToList);
	stdButtonSizer->CreateButton(this, buttons, StdButton::Apply, wxID_APPLY, addToList);
	stdButtonSizer->CreateButton(this, buttons, StdButton::Close, wxID_CLOSE, addToList);
	stdButtonSizer->CreateButton(this, buttons, StdButton::Cancel, wxID_CANCEL, addToList);
	stdButtonSizer->CreateButton(this, buttons, StdButton::Help, wxID_HELP, addToList);

	if (defaultButtons & StdButton::No)
	{
		if (no)
		{
			no->SetDefault();
			no->SetFocus();
		}
	}
	else
	{
		if (ok)
		{
			ok->SetDefault();
			ok->SetFocus();
		}
		else if (yes)
		{
			yes->SetDefault();
			yes->SetFocus();
		}
	}

	if (buttons & StdButton::OK)
	{
		SetAffirmativeId(wxID_OK);
	}
	else if (buttons & StdButton::Yes)
	{
		SetAffirmativeId(wxID_YES);
	}
	else if (buttons & StdButton::Close)
	{
		SetAffirmativeId(wxID_CLOSE);
	}

	stdButtonSizer->Realize();
	return stdButtonSizer;
}
void KxDialog::SetStdLabels(wxSizer* sizer)
{
	for (auto& sizerItem: sizer->GetChildren())
	{
		if (wxWindow* window = sizerItem->GetWindow())
		{
			window->SetLabel(Localization::GetStandardLocalizedString(window->GetId()));

			wxSize size = window->GetSize();
			size.Scale(0.85f, 1.0f);
			size.SetHeight(23);
			window->SetMaxSize(size);
		}
	}
}
void KxDialog::CenterIfNoPosition(const wxPoint& pos)
{
	if (!pos.IsFullySpecified())
	{
		CenterOnScreen(wxBOTH);
	}
}
