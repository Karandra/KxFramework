#include "KxStdAfx.h"
#include "FileType.h"
#include "FileTypeManager.h"
#include <KxFramework/KxString.h>
#include <KxFramework/KxRegistry.h>
#include <KxFramework/KxComparator.h>

wxString KxFileType::GetOpenExecutable() const
{
	wxString openCommand = GetOpenCommand(wxEmptyString);
	if (wxRegEx regEx(u8R"(\"(.+?)\")", wxRE_ADVANCED|wxRE_ICASE); regEx.Matches(openCommand))
	{
		return regEx.GetMatch(openCommand, 1);
	}
	return wxEmptyString;
}

bool KxFileType::IsURLProtocol(const wxString& extension) const
{
	if (m_FileType && !extension.IsEmpty())
	{
		wxString extWithoutDot = KxFileTypeManager::NormalizeFileExtension(extension);
		for (const wxString& ext: GetAllExtensions())
		{
			if (KxComparator::IsEqual(ext, extWithoutDot, true))
			{
				return !KxRegistry::GetValue(KxREG_HKEY_CLASSES_ROOT, extWithoutDot, wxS("URL Protocol"), KxREG_VALUE_SZ).IsNull();
			}
		}
	}
	return false;
}
