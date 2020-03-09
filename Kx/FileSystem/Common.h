#pragma once
#include "Kx/Common.hpp"
#include <wx/wx.h>

namespace KxFileSystem
{
	enum class PathNamespace
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
	enum class DriveType
	{
		Unknown = 0,
		NotMounted,
		Fixed,
		Remote,
		Memory,
		Optical,
		Removable,
	};
}

namespace KxFileSystem
{
	wxString GetNamespaceString(PathNamespace type);
	wxString GetForbiddenChars();
}
