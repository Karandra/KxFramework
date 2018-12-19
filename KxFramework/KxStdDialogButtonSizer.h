/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxDialog;
class KX_API KxButton;
class KX_API KxStdDialogButtonSizer: public wxBoxSizer
{
	private:
		std::vector<wxAnyButton*> m_NonStandardButtons;
		bool m_HasFirstPrepend = false;

	protected:
		wxAnyButton* m_ButtonAffirmative = nullptr; // wxID_OK, wxID_YES, wxID_SAVE go here
		wxAnyButton* m_ButtonApply = nullptr; // wxID_APPLY
		wxAnyButton* m_ButtonNegative = nullptr; // wxID_NO
		wxAnyButton* m_ButtonCancel = nullptr; // wxID_CANCEL, wxID_CLOSE
		wxAnyButton* m_ButtonHelp = nullptr; // wxID_HELP, wxID_CONTEXT_HELP

	private:
		int GetSpacing(wxAnyButton* button)
		{
			return button->ConvertDialogToPixels(wxSize(2, 0)).GetX();
		}
		void ConfigureButton(wxAnyButton* button);

	public:
		// Constructor just creates a new wxBoxSizer, not much else.
		// Box sizer orientation is automatically determined here:
		// vertical for PDAs, horizontal for everything else?
		KxStdDialogButtonSizer();

	public:
		KxButton* CreateButton(KxDialog* dialog, int buttons, int buttonConstant, wxWindowID id, bool addToList = true);
		
		// Checks button ID against system IDs and sets one of the pointers below
		// to this button. Does not do any sizer-related things here.
		bool AddButton(wxAnyButton* button);
		void AddCustomButton(KxButton* button, const wxString& label = wxEmptyString, bool prepend = false);

		// Use these if no standard ID can/should be used
		void SetAffirmativeButton(wxAnyButton* button)
		{
			m_ButtonAffirmative = button;
		}
		void SetNegativeButton(wxAnyButton* button)
		{
			m_ButtonNegative = button;
		}
		void SetCancelButton(wxAnyButton* button)
		{
			m_ButtonCancel = button;
		}

		wxAnyButton* GetAffirmativeButton() const
		{
			return m_ButtonAffirmative;
		}
		wxAnyButton* GetApplyButton() const
		{
			return m_ButtonApply;
		}
		wxAnyButton* GetNegativeButton() const
		{
			return m_ButtonNegative;
		}
		wxAnyButton* GetCancelButton() const
		{
			return m_ButtonCancel;
		}
		wxAnyButton* GetHelpButton() const
		{
			return m_ButtonHelp;
		}

		// All platform-specific code here, checks which buttons exist and add them to the sizer accordingly.
		virtual void Realize();

	public:
		wxDECLARE_CLASS(KxStdDialogButtonSizer);
		wxDECLARE_NO_COPY_CLASS(KxStdDialogButtonSizer);
};

