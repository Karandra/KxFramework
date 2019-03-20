#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxTextBox: public wxTextCtrl
{
	private:
		virtual WXLRESULT MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

	public:
		static const long DefaultStyle = wxTE_LEFT;
		static const wxString DefaultPasswordChar;

		KxTextBox() {}
		KxTextBox(wxWindow* parent,
				  wxWindowID id,
				  const wxString& value = wxEmptyString,
				  long style = DefaultStyle,
				  const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, value, style, validator);
		}
		KxTextBox(wxWindow* parent,
				  wxWindowID id,
				  const wxString& value,
				  const wxPoint& pos,
				  const wxSize& size,
				  long style = DefaultStyle,
				  const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, value, pos, size, style, validator);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& value = wxEmptyString,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		)
		{
			return Create(parent, id, value, wxDefaultPosition, wxDefaultSize, style, validator);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& value,
					const wxPoint& pos,
					const wxSize& size,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);

	public:
		bool SetTabWidth(unsigned int width = 16);
		void SetLimit(size_t max);
		
		void SetPasswordChar(const wxString& character = DefaultPasswordChar);
		virtual bool SetHint(const wxString& label) override;
		
		virtual void SetValue(const wxString& value) override;
		void SetValueEvent(const wxString& value);
		
		virtual void Clear() override;
		void ClearEvent();

		wxDECLARE_DYNAMIC_CLASS(KxTextBox);
};
