#include "stdafx.h"
#include "ComboBox.h"
#include <CommCtrl.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ComboBox, wxComboBox);

	bool ComboBox::Create(wxWindow* parent,
						  wxWindowID id,
						  const String& value,
						  const wxPoint& pos,
						  const wxSize& size,
						  ComboBoxStyle style,
						  const wxValidator& validator
	)
	{
		if (wxComboBox::Create(parent, id, value, pos, size, 0, nullptr, ToInt(style), validator))
		{
			EnableSystemTheme();
			return true;
		}
		return false;
	}

	bool ComboBox::SetVisibleItemsCount(size_t count)
	{
		return ::SendMessageW(GetHandle(), CB_SETMINVISIBLE, (WPARAM)count, 0);
	}
}
