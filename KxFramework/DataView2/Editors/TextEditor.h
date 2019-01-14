#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Editor.h"

namespace Kx::DataView2
{
	class KX_API TextEditor: public Editor
	{
		protected:
			virtual wxWindow* CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
			virtual wxAny GetValue(wxWindow* control) const override;
	};
}
