#include "KxStdAfx.h"
#include "Common.h"
#include "NamespacePrefix.h"

namespace KxFramework::FileSystem
{
	wxString GetNamespaceString(FSPathNamespace type)
	{
		switch (type)
		{
			case FSPathNamespace::NT:
			{
				return NamespacePrefix::NT;
			}
			case FSPathNamespace::Win32File:
			{
				return NamespacePrefix::Win32File;
			}
			case FSPathNamespace::Win32FileUNC:
			{
				return NamespacePrefix::Win32FileUNC;
			}
			case FSPathNamespace::Win32Device:
			{
				return NamespacePrefix::Win32Device;
			}
			case FSPathNamespace::Network:
			{
				return NamespacePrefix::Network;
			}
			case FSPathNamespace::NetworkUNC:
			{
				return NamespacePrefix::NetworkUNC;
			}
		};
		return wxEmptyString;
	}
	wxString GetForbiddenCharsExceptSeparators(const wxString& except)
	{
		wxString forbiddenChars = wxFileName::GetForbiddenChars();
		for (wxChar c: except)
		{
			forbiddenChars.Replace(c, wxEmptyString, true);
		}
		return forbiddenChars;
	}
}
