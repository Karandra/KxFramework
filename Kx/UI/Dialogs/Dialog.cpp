#include "stdafx.h"
#include "Dialog.h"
#include "Kx/UI/Controls/Button.h"
#include "Kx/Localization/Common.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(Dialog, wxDialog);

	bool Dialog::Create(wxWindow* parent,
						wxWindowID id,
						const String& title,
						const Point& pos,
						const Size& size,
						DialogStyle style
	)
	{
		if (wxDialog::Create(parent, id, title, pos, size, ToInt(style)))
		{
			SetInitialSize(size);
			SetDefaultBackgroundColor();
			CenterIfNoPosition(pos);

			return true;
		}
		return false;
	}

	StdDialogButtonSizer* Dialog::CreateStdDialogButtonSizer(StdButton buttons, StdButton defaultButtons)
	{
		auto* stdButtonSizer = new UI::StdDialogButtonSizer();

		const bool addToList = true;
		Button* ok = stdButtonSizer->CreateButton(this, buttons, StdButton::OK, wxID_OK, addToList);
		Button* yes = stdButtonSizer->CreateButton(this, buttons, StdButton::Yes, wxID_YES, addToList);
		Button* no = stdButtonSizer->CreateButton(this, buttons, StdButton::No, wxID_NO, addToList);
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
	void Dialog::SetStdLabels(wxSizer* sizer)
	{
		for (auto& sizerItem: sizer->GetChildren())
		{
			if (wxWindow* window = sizerItem->GetWindow())
			{
				window->SetLabel(Localization::GetStandardLocalizedString(window->GetId()));

				Size size = window->GetSize();
				size.Scale(0.85f, 1.0f);
				size.SetHeight(23);
				window->SetMaxSize(size);
			}
		}
	}
	void Dialog::CenterIfNoPosition(const Point& pos)
	{
		if (!pos.IsFullySpecified())
		{
			CenterOnScreen(wxBOTH);
		}
	}
}
