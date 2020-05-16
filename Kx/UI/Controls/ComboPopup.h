#pragma once
#include "Kx/UI/Common.h"
#include <wx/combo.h>

namespace KxFramework::UI
{
	class ComboControl;
}

namespace KxFramework::UI
{
	class KX_API ComboPopup: public wxComboPopup
	{
		friend class ComboControl;

		protected:
			virtual void OnDoShowPopup()
			{
			}
	};
}
