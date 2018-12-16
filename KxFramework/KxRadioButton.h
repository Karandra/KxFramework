#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxRadioButton: public wxRadioButton
{
	public:
		static const long DefaultStyle = 0;

		KxRadioButton() {}
		KxRadioButton(wxWindow* parent,
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
		wxDECLARE_DYNAMIC_CLASS(KxRadioButton);
};
