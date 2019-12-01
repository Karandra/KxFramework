#include "KxStdAfx.h"
#include "FileTypeInfo.h"
#include "FileTypeManager.h"

bool KxFileTypeInfo::IsURLProtocol(const wxString& ext) const
{
	auto it = m_URLProtocolMap.find(KxFileTypeManager::NormalizeFileExtension(ext));
	if (it != m_URLProtocolMap.end())
	{
		return it->second;
	}
	return false;
}
KxFileTypeInfo& KxFileTypeInfo::SetURLProtocol(const wxString& ext, bool protocol)
{
	m_URLProtocolMap.insert_or_assign(KxFileTypeManager::NormalizeFileExtension(ext), protocol);
	return *this;
}
