#pragma once
#include <wx/wx.h>

namespace KxSciter::Internal
{
	std::vector<BYTE> ToSciterUTF8(const wxString& text);
}
