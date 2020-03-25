#pragma once
#include <wx/wx.h>

namespace KxFramework::FileSystem::NamespacePrefix
{
	constexpr wxChar NT[] = wxS("\\");
	constexpr wxChar Win32File[] = wxS("\\\\?\\");
	constexpr wxChar Win32FileUNC[] = wxS("\\\\?\\UNC\\");
	constexpr wxChar Win32Volume[] = wxS("\\\\?\\Volume");
	constexpr wxChar Win32Device[] = wxS("\\\\.\\");
	constexpr wxChar Network[] = wxS("\\\\");
	constexpr wxChar NetworkUNC[] = wxS("\\\\?\\UNC\\");
}
