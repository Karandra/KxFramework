#pragma once
#include <wx/wx.h>

namespace kxf::FileSystem::Private::NamespacePrefix
{
	constexpr char NT[] = "\\";
	constexpr char Win32File[] = "\\\\?\\";
	constexpr char Win32FileUNC[] = "\\\\?\\UNC\\";
	constexpr char Win32Device[] = "\\\\.\\";
	constexpr char Network[] = "\\\\";
	constexpr char NetworkUNC[] = "\\\\?\\UNC\\";
}
