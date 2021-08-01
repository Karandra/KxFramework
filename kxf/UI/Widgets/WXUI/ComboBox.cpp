#include "KxfPCH.h"
#include "ComboBox.h"
#include <CommCtrl.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::WXUI
{
	bool ComboBox::Create(wxWindow* parent,
						const String& label,
						const Point& pos,
						const Size& size
	)
	{
		if (wxComboBox::Create(parent, wxID_NONE, {}, pos, size, 0, nullptr, wxCB_DROPDOWN|wxCB_READONLY|wxTE_PROCESS_ENTER, wxDefaultValidator))
		{
			EnableSystemTheme();
			return true;
		}
		return false;
	}

	bool ComboBox::SetVisibleItemCount(size_t count)
	{
		return ::SendMessageW(GetHandle(), CB_SETMINVISIBLE, static_cast<WPARAM>(count), 0) != 0;
	}
}
