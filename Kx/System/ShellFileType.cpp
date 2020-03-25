#include "KxStdAfx.h"
#include "ShellFileType.h"
#include "ShellFileTypeManager.h"
#include <KxFramework/KxString.h>
#include <KxFramework/KxRegistry.h>
#include <KxFramework/KxComparator.h>

namespace KxFramework
{
	wxString ShellFileType::GetOpenExecutable() const
	{
		wxString openCommand = GetOpenCommand(wxEmptyString);
		if (wxRegEx regEx(u8R"(\"(.+?)\")", wxRE_ADVANCED|wxRE_ICASE); regEx.Matches(openCommand))
		{
			return regEx.GetMatch(openCommand, 1);
		}
		return wxEmptyString;
	}

	bool ShellFileType::IsURLProtocol(const wxString& extension) const
	{
		if (m_FileType && !extension.IsEmpty())
		{
			wxString extWithoutDot = ShellFileTypeManager::NormalizeFileExtension(extension);
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
}
