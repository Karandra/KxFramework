#include "KxStdAfx.h"
#include "FileTypeManager.h"
#include <KxFramework/KxString.h>

bool KxFileTypeManager::IsAssociatedWith(const KxFileType& fileType, const wxString& executablePath) const
{
	if (fileType)
	{
		const wxString executablePathL = KxString::ToLower(executablePath);
		const wxString openCommand = KxString::ToLower(fileType.GetOpenCommand(wxEmptyString));
		if (openCommand == executablePathL)
		{
			return true;
		}
		else if (wxRegEx regEx(u8R"(\"(.+?)\")", wxRE_ADVANCED); regEx.Matches(openCommand))
		{
			return regEx.GetMatch(openCommand, 1) == executablePathL;
		}
	}
	return false;
}
