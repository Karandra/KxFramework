#include "KxStdAfx.h"
#include "Common.h"
#include "NamespacePrefix.h"

namespace KxFileSystem
{
	wxString GetNamespaceString(KxFSPathNamespace type)
	{
		switch (type)
		{
			case KxFSPathNamespace::NT:
			{
				return NamespacePrefix::NT;
			}
			case KxFSPathNamespace::Win32File:
			{
				return NamespacePrefix::Win32File;
			}
			case KxFSPathNamespace::Win32FileUNC:
			{
				return NamespacePrefix::Win32FileUNC;
			}
			case KxFSPathNamespace::Win32Device:
			{
				return NamespacePrefix::Win32Device;
			}
			case KxFSPathNamespace::Win32Volume:
			{
				return NamespacePrefix::Win32Volume;
			}
			case KxFSPathNamespace::Network:
			{
				return NamespacePrefix::Network;
			}
			case KxFSPathNamespace::NetworkUNC:
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
