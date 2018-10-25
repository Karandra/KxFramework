/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxZLibStream.h"

wxString KxZLib::GetLibraryName()
{
	return wxGetZlibVersionInfo().GetName();
}
wxString KxZLib::GetLibraryVersion()
{
	return wxGetZlibVersionInfo().GetVersionString();
}
