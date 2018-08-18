#pragma once
#include "KxFramework/KxFramework.h"
class KxComboControl;

class KxComboPopup: public wxComboPopup
{
	friend class KxComboControl;

	protected:
		virtual void OnDoShowPopup()
		{
		}
};
