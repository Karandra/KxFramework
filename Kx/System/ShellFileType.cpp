#include "KxStdAfx.h"
#include "ShellFileType.h"
#include "ShellFileTypeManager.h"
#include <KxFramework/KxString.h>
#include <KxFramework/KxRegistry.h>

namespace KxFramework
{
	FSPath ShellFileType::GetOpenExecutable() const
	{
		String openCommand = GetOpenCommand(NullString);
		if (wxRegEx regEx(u8R"(\"(.+?)\")", wxRE_ADVANCED|wxRE_ICASE); regEx.Matches(openCommand))
		{
			return regEx.GetMatch(openCommand, 1);
		}
		return {};
	}

	bool ShellFileType::IsURLProtocol(const FSPath& extension) const
	{
		if (m_FileType && extension)
		{
			for (const String& ext: GetAllExtensions())
			{
				if (ext == extension)
				{
					return !KxRegistry::GetValue(KxREG_HKEY_CLASSES_ROOT, extension.GetExtension(), wxS("URL Protocol"), KxREG_VALUE_SZ).IsNull();
				}
			}
		}
		return false;
	}
}
