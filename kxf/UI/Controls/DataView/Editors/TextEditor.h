#pragma once
#include "../Editor.h"

namespace kxf::UI::DataView
{
	class KX_API TextEditor: public Editor
	{
		protected:
			wxWindow* CreateControl(wxWindow* parent, const Rect& cellRect, const wxAny& value) override;
			wxAny GetValue(wxWindow* control) const override;
	};
}
