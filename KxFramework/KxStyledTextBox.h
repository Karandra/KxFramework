#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxStyledTextBox: public wxStyledTextCtrl
{
	private:
		bool m_IsMultiLine = true;

	public:
		static const long DefaultStyle = 0;
		
		KxStyledTextBox() {}
		KxStyledTextBox(wxWindow* parent,
						wxWindowID id,
						long style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, style, validator);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);
		virtual ~KxStyledTextBox();
	
	public:
		void SetModified(bool modified = true)
		{
			modified ? wxStyledTextCtrl::MarkDirty() : wxStyledTextCtrl::SetSavePoint();
		}
		
		bool IsMultiLine() const
		{
			return m_IsMultiLine;
		}
		bool IsSingleLine() const
		{
			return !IsMultiLine();
		}
		void SetMultiLine(bool value)
		{
			m_IsMultiLine = value;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStyledTextBox);
};
