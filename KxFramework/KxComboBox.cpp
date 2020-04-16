#include "KxStdAfx.h"
#include "KxFramework/KxComboBox.h"
#include <CommCtrl.h>
#include "Kx/System/UndefWindows.h"

bool KxComboBox::Create(wxWindow* parent,
						wxWindowID id,
						const wxString& value,
						const wxPoint& pos,
						const wxSize& size,
						long style,
						const wxValidator& validator
)
{
	if (wxComboBox::Create(parent, id, value, pos, size, 0, nullptr, style, validator))
	{
		EnableSystemTheme();
		return true;
	}
	return false;
}

void KxComboBox::SetVisibleItemsCount(size_t count)
{
	::SendMessageW(GetHandle(), CB_SETMINVISIBLE, (WPARAM)count, 0);
}
