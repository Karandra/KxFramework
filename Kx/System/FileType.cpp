#include "KxStdAfx.h"
#include "FileType.h"
#include <KxFramework/KxString.h>

wxString KxFileType::GetOpenExecutable() const
{
	wxString openCommand = GetOpenCommand(wxEmptyString);
	if (wxRegEx regEx(u8R"(\"(.+?)\")", wxRE_ADVANCED|wxRE_ICASE); regEx.Matches(openCommand))
	{
		return regEx.GetMatch(openCommand, 1);
	}
	return wxEmptyString;
}
