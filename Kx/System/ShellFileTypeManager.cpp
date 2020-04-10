#include "KxStdAfx.h"
#include "ShellFileTypeManager.h"
#include "Kx/General/StringFormater.h"
#include <KxFramework/KxRegistry.h>

namespace KxFramework
{
	ShellFileType ShellFileTypeManager::FileTypeFromExtension(const String& extension) const
	{
		return m_Manager.GetFileTypeFromExtension(extension);
	}
	ShellFileType ShellFileTypeManager::FileTypeFromMimeType(const String& mimeType) const
	{
		return m_Manager.GetFileTypeFromMimeType(mimeType);
	}

	ShellFileType ShellFileTypeManager::Associate(const ShellFileTypeInfo& fileTypeInfo)
	{
		if (wxFileType* fileType = m_Manager.Associate(fileTypeInfo.AsWxFileTypeInfo()))
		{
			for (const String& extension: fileTypeInfo.GetExtensions())
			{
				String ext = FSPath(extension).GetExtension();
				if (fileTypeInfo.IsURLProtocol(ext))
				{
					KxRegistry::SetValue(KxREG_HKEY_CLASSES_ROOT, ext, wxS("URL Protocol"), {}, KxREG_VALUE_SZ);
					KxRegistry::SetValue(KxREG_HKEY_CLASSES_ROOT, ext, {}, String::Format(wxS("URL:%1 Protocol"), ext.MakeUpper()), KxREG_VALUE_SZ);
				}
			}
			return fileType;
		}
		return nullptr;
	}
	bool ShellFileTypeManager::IsAssociatedWith(const ShellFileType& fileType, const FSPath& executablePath) const
	{
		if (fileType)
		{
			return fileType.GetOpenExecutable() == executablePath;
		}
		return false;
	}
	bool ShellFileTypeManager::Unassociate(ShellFileType& fileType)
	{
		if (fileType)
		{
			return m_Manager.Unassociate(&fileType.AsWxFileType());
		}
		return false;
	}
}
