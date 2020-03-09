#include "KxStdAfx.h"
#include "Common.h"
#include "NamespacePrefix.h"

namespace KxFileSystem
{
	wxString GetNamespaceString(PathNamespace type)
	{
		switch (type)
		{
			case PathNamespace::NT:
			{
				return NamespacePrefix::NT;
			}
			case PathNamespace::Win32File:
			{
				return NamespacePrefix::Win32File;
			}
			case PathNamespace::Win32FileUNC:
			{
				return NamespacePrefix::Win32FileUNC;
			}
			case PathNamespace::Win32Device:
			{
				return NamespacePrefix::Win32Device;
			}
			case PathNamespace::Win32Volume:
			{
				return NamespacePrefix::Win32Volume;
			}
			case PathNamespace::Network:
			{
				return NamespacePrefix::Network;
			}
			case PathNamespace::NetworkUNC:
			{
				return NamespacePrefix::NetworkUNC;
			}
		};
		return wxEmptyString;
	}
	wxString GetForbiddenChars()
	{
		return wxFileName::GetForbiddenChars();
	}
}
