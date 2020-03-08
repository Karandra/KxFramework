#pragma once
#include "Kx/Common.hpp"
#include <wx/wx.h>

enum class KxFSPathNamespace
{
	None = 0,
	NT,
	Win32File,
	Win32FileUNC,
	Win32Device,
	Win32Volume,
	Network,
	NetworkUNC,
};
namespace KxFileSystem
{
	wxString GetNamespaceString(KxFSPathNamespace type);
	wxString GetForbiddenChars();
}
