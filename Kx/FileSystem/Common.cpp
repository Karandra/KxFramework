#include "stdafx.h"
#include "Common.h"
#include "Kx/General/String.h"
#include "Private/NamespacePrefix.h"

namespace KxFramework::FileSystem
{
	String GetNamespaceString(FSPathNamespace type)
	{
		using namespace Private;

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
		return {};
	}
	String GetForbiddenChars(const String& except)
	{
		String forbiddenChars = wxFileName::GetForbiddenChars();
		for (XChar c: except)
		{
			forbiddenChars.Replace(c, NullString);
		}
		return forbiddenChars;
	}
}
