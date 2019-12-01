#include "KxStdAfx.h"
#include "FileTypeManager.h"
#include <KxFramework/KxComparator.h>
#include <KxFramework/KxRegistry.h>
#include <KxFramework/KxString.h>

wxString KxFileTypeManager::NormalizeFileExtension(const wxString& extension)
{
	if (!extension.IsEmpty() && *extension.begin() == wxS('.'))
	{
		return wxString(extension).Remove(0, 1);
	}
	return extension;
}

KxFileType KxFileTypeManager::FileTypeFromExtension(const wxString& extension) const
{
	return m_Manager.GetFileTypeFromExtension(extension);
}
KxFileType KxFileTypeManager::FileTypeFromMimeType(const wxString& mimeType) const
{
	return m_Manager.GetFileTypeFromMimeType(mimeType);
}

KxFileType KxFileTypeManager::Associate(const KxFileTypeInfo& fileTypeInfo)
{
	if (wxFileType* fileType = m_Manager.Associate(fileTypeInfo.AsWxFileTypeInfo()))
	{
		for (const wxString& ext: fileTypeInfo.GetExtensions())
		{
			if (fileTypeInfo.IsURLProtocol(ext))
			{
				wxString extWithoutDot = NormalizeFileExtension(ext);

				KxRegistry::SetValue(KxREG_HKEY_CLASSES_ROOT, extWithoutDot, wxS("URL Protocol"), {}, KxREG_VALUE_SZ);
				KxRegistry::SetValue(KxREG_HKEY_CLASSES_ROOT, extWithoutDot, {}, KxString::Format(wxS("URL:%1 Protocol"), KxString::ToUpper(extWithoutDot)), KxREG_VALUE_SZ);
			}
		}
		return fileType;
	}
	return nullptr;
}
bool KxFileTypeManager::IsAssociatedWith(const KxFileType& fileType, const wxString& executablePath) const
{
	if (fileType)
	{
		return KxComparator::IsEqual(fileType.GetOpenExecutable(), executablePath, true);
	}
	return false;
}
bool KxFileTypeManager::Unassociate(KxFileType& fileType)
{
	if (fileType)
	{
		return m_Manager.Unassociate(&fileType.AsWxFileType());
	}
	return false;
}
