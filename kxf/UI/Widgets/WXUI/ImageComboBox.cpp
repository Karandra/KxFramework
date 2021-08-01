#include "KxfPCH.h"
#include "ImageComboBox.h"
#include <CommCtrl.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::WXUI
{
	bool ImageComboBox::Create(wxWindow* parent,
							   const String& label,
							   const Point& pos,
							   const Size& size
	)
	{
		if (wxBitmapComboBox::Create(parent, wxID_NONE, {}, pos, size, 0, nullptr, wxCB_READONLY|wxTE_PROCESS_ENTER, wxDefaultValidator))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);

			return true;
		}
		return false;
	}

	bool ImageComboBox::SetVisibleItemCount(size_t count)
	{
		return ::SendMessageW(GetHandle(), CB_SETMINVISIBLE, static_cast<WPARAM>(count), 0) != 0;
	}
}
