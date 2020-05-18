#pragma once
#include "../Editor.h"

namespace KxFramework::UI::DataView
{
	class KX_API TextEditor: public Editor
	{
		protected:
			wxWindow* CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
			wxAny GetValue(wxWindow* control) const override;
	};
}
