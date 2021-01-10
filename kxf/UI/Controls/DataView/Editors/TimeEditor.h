#pragma once
#include "../Editor.h"

namespace kxf::UI::DataView
{
	class KX_API TimeEditor: public Editor
	{
		protected:
			wxWindow* CreateControl(wxWindow* parent, const Rect& cellRect, const Any& value) override;
			Any GetValue(wxWindow* control) const override;
	};
}
