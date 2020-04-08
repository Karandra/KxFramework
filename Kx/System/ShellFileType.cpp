#include "KxStdAfx.h"
#include "ShellFileType.h"
#include "ShellFileTypeManager.h"
#include <KxFramework/KxString.h>
#include <KxFramework/KxRegistry.h>

namespace KxFramework
{
	String ShellFileType::GetOpenExecutable() const
	{
		String openCommand = GetOpenCommand(wxEmptyString);
		if (wxRegEx regEx(u8R"(\"(.+?)\")", wxRE_ADVANCED|wxRE_ICASE); regEx.Matches(openCommand))
		{
			return regEx.GetMatch(openCommand, 1);
		}
		return wxEmptyString;
	}

	bool ShellFileType::IsURLProtocol(const String& extension) const
	{
		if (m_FileType && !extension.IsEmpty())
		{
			String extWithoutDot = ShellFileTypeManager::NormalizeFileExtension(extension);
			for (const String& ext: GetAllExtensions())
			{
				if (ext.IsSameAs(extWithoutDot, StringOpFlag::IgnoreCase))
				{
					return !KxRegistry::GetValue(KxREG_HKEY_CLASSES_ROOT, extWithoutDot, wxS("URL Protocol"), KxREG_VALUE_SZ).IsNull();
				}
			}
		}
		return false;
	}
}
