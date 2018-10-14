#pragma once
#include "KxFramework/KxFramework.h"

class KxSearchBox: public wxSearchCtrl
{
	private:
		wxEvtHandler m_EvtHandler;
		wxString m_Value;

	private:
		void OnText(wxCommandEvent& event);
		void OnEnter(wxCommandEvent& event);
		void OnKillFocus(wxFocusEvent& event);
		void OnCancelSearch(wxCommandEvent& event);

	public:
		static const long DefaultStyle = wxTE_LEFT|wxTE_PROCESS_ENTER|wxTE_NOHIDESEL;
		static const wxString DefaultPasswordChar;

		KxSearchBox() {}
		KxSearchBox(wxWindow* parent,
					wxWindowID id,
					const wxString& value = wxEmptyString,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, value, style, validator);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& value = wxEmptyString,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);
		virtual ~KxSearchBox();

	public:
		wxDECLARE_DYNAMIC_CLASS(KxTextBox);
};
