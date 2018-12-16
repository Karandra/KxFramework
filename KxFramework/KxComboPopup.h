#pragma once
#include "KxFramework/KxFramework.h"
class KX_API KxComboControl;

class KX_API KxComboPopup: public wxComboPopup
{
	friend class KxComboControl;

	protected:
		virtual void OnDoShowPopup()
		{
		}
};
