#include "KxStdAfx.h"
#include "ShellFileTypeManager.h"
#include "Registry.h"
#include "Kx/General/StringFormater.h"

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
					RegistryKey classesRoot(RegistryBaseKey::ClassesRoot, {}, RegistryAccess::Create);
					if (classesRoot)
					{
						RegistryKey key = classesRoot.CreateKey(ext, RegistryAccess::Write);
						key.SetStringValue({}, String::Format(wxS("URL:%1 Protocol"), ext.MakeUpper()));
						key.SetStringValue(wxS("URL Protocol"), {});
					}
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
