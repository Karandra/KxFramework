#include "KxStdAfx.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxButton.h"
#include "KxFramework/KxStdDialogButtonSizer.h"
#include "KxFramework/KxUtility.h"

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

KxStdDialogButtonSizer* KxDialog::CreateStdDialogButtonSizer(int buttons)
{
	KxStdDialogButtonSizer* stdButtonSizer = new KxStdDialogButtonSizer();

	const bool addToList = true;
	KxButton* ok = stdButtonSizer->CreateButton(this, buttons, KxBTN_OK, wxID_OK, addToList);
	KxButton* yes = stdButtonSizer->CreateButton(this, buttons, KxBTN_YES, wxID_YES, addToList);
	KxButton* no = stdButtonSizer->CreateButton(this, buttons, KxBTN_NO, wxID_NO, addToList);
	stdButtonSizer->CreateButton(this, buttons, KxBTN_RETRY, wxID_RETRY, addToList);
	stdButtonSizer->CreateButton(this, buttons, KxBTN_APPLY, wxID_APPLY, addToList);
	stdButtonSizer->CreateButton(this, buttons, KxBTN_CLOSE, wxID_CLOSE, addToList);
	stdButtonSizer->CreateButton(this, buttons, KxBTN_CANCEL, wxID_CANCEL, addToList);
	stdButtonSizer->CreateButton(this, buttons, KxBTN_HELP, wxID_HELP, addToList);

	if (buttons & wxNO_DEFAULT)
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

	if (buttons & wxOK)
	{
		SetAffirmativeId(wxID_OK);
	}
	else if (buttons & wxYES)
	{
		SetAffirmativeId(wxID_YES);
	}
	else if (buttons & wxCLOSE)
	{
		SetAffirmativeId(wxID_CLOSE);
	}

	stdButtonSizer->Realize();
	return stdButtonSizer;
}
void KxDialog::SetStdLabels(wxSizer* sizer)
{
	auto list = sizer->GetChildren();
	for (size_t i = 0; i < list.size(); i++)
	{
		wxWindow* window = list[i]->GetWindow();
		if (window != NULL)
		{
			wxString label = KxUtility::GetStandardLocalizedString(window->GetId());
			window->SetLabel(label);

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
