#include "KxStdAfx.h"
#include "ShellFileTypeInfo.h"
#include "ShellFileTypeManager.h"

namespace KxFramework
{
	bool ShellFileTypeInfo::IsURLProtocol(const String& ext) const
	{
		auto it = m_URLProtocolMap.find(ShellFileTypeManager::NormalizeFileExtension(ext));
		if (it != m_URLProtocolMap.end())
		{
			return it->second;
		}
		return false;
	}
	ShellFileTypeInfo& ShellFileTypeInfo::SetURLProtocol(const String& ext, bool protocol)
	{
		m_URLProtocolMap.insert_or_assign(ShellFileTypeManager::NormalizeFileExtension(ext), protocol);
		return *this;
	}
}
