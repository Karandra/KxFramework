#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxCheckBox: public wxCheckBox
{
	private:
		void Set3StateValue(wxCheckBoxState) = delete;
		wxCheckBoxState Get3StateValue() = delete;

	public:
		static const long DefaultStyle = wxCHK_2STATE;

		KxCheckBox() {}
		KxCheckBox(wxWindow* parent,
				   wxWindowID id,
				   const wxString& label,
				   long style = DefaultStyle
		)
		{
			Create(parent, id, label, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& label,
					long style = DefaultStyle
		);

	public:
		virtual bool GetValue() const override
		{
			if (Is3State())
			{
				return wxCheckBox::Get3StateValue() == wxCHK_CHECKED;
			}
			return wxCheckBox::GetValue();
		}
		virtual wxCheckBoxState GetCheckState() const
		{
			if (Is3State())
			{
				return wxCheckBox::Get3StateValue();
			}
			return wxCheckBox::GetValue() ? wxCHK_CHECKED : wxCHK_UNCHECKED;
		}

		virtual void SetValue(bool value) override
		{
			wxCheckBox::SetValue(value);
		}
		virtual void SetCheckState(wxCheckBoxState state)
		{
			wxCheckBox::Set3StateValue(state);
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxCheckBox);
};
