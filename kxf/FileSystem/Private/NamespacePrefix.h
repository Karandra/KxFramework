#pragma once
#include <wx/wx.h>

namespace kxf::FileSystem::Private::NamespacePrefix
{
	constexpr XChar NT[] = kxS("\\");
	constexpr XChar Win32File[] = kxS("\\\\?\\");
	constexpr XChar Win32FileUNC[] = kxS("\\\\?\\UNC\\");
	constexpr XChar Win32Device[] = kxS("\\\\.\\");
	constexpr XChar Network[] = kxS("\\\\");
	constexpr XChar NetworkUNC[] = kxS("\\\\?\\UNC\\");

	// Not strictly namespaces, but useful
	constexpr XChar DotRelative1[] = kxS(".\\");
	constexpr XChar DotRelative2[] = kxS("..\\");
	constexpr XChar DotRelative3[] = kxS("./");
	constexpr XChar DotRelative4[] = kxS("..//");
	constexpr XChar DotRelative5[] = kxS("\\.");
	constexpr XChar DotRelative6[] = kxS("..");
}
