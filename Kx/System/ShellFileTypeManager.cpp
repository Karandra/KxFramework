#include "KxStdAfx.h"
#include "ShellFileTypeManager.h"
#include <KxFramework/KxComparator.h>
#include <KxFramework/KxRegistry.h>
#include <KxFramework/KxString.h>

namespace KxFramework
{
	String ShellFileTypeManager::NormalizeFileExtension(const String& extension)
	{
		if (!extension.IsEmpty() && *extension.begin() == wxS('.'))
		{
			return String(extension).Remove(0, 1);
		}
		return extension;
	}

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
			for (const String& ext: fileTypeInfo.GetExtensions())
			{
				if (fileTypeInfo.IsURLProtocol(ext))
				{
					String extWithoutDot = NormalizeFileExtension(ext);

					KxRegistry::SetValue(KxREG_HKEY_CLASSES_ROOT, extWithoutDot, wxS("URL Protocol"), {}, KxREG_VALUE_SZ);
					KxRegistry::SetValue(KxREG_HKEY_CLASSES_ROOT, extWithoutDot, {}, KxString::Format(wxS("URL:%1 Protocol"), KxString::ToUpper(extWithoutDot)), KxREG_VALUE_SZ);
				}
			}
			return fileType;
		}
		return nullptr;
	}
	bool ShellFileTypeManager::IsAssociatedWith(const ShellFileType& fileType, const String& executablePath) const
	{
		if (fileType)
		{
			return KxComparator::IsEqual(fileType.GetOpenExecutable(), executablePath, true);
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
