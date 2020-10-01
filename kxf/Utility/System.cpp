#include "stdafx.h"
#include "System.h"
#include "Common.h"
#include "kxf/System/DynamicLibrary.h"

namespace
{
	wxScopedCharBuffer DoLoadResource(HRSRC resourceHandle, HMODULE moduleHandle)
	{
		size_t size = ::SizeofResource(moduleHandle, resourceHandle);
		if (size != 0)
		{
			HGLOBAL resHandle = ::LoadResource(moduleHandle, resourceHandle);
			if (const void* data = ::LockResource(resHandle))
			{
				return wxScopedCharBuffer::CreateNonOwned(reinterpret_cast<const char*>(data), size);
			}
		}
		return {};
	}
}

namespace kxf::Utility
{
	const wxScopedCharBuffer LoadResource(const DynamicLibrary& library, const String& resourceName, const String& resourceType)
	{
		if (library)
		{
			const HMODULE moduleHandle = static_cast<HMODULE>(library.GetHandle());
			const HRSRC resourceHandle = ::FindResourceW(moduleHandle, resourceName.wc_str(), resourceType.wc_str());
			return DoLoadResource(resourceHandle, moduleHandle);
		}
		return {};
	}
	const wxScopedCharBuffer LoadResource(const DynamicLibrary& library, int resourceID, const String& resourceType)
	{
		if (library)
		{
			const HMODULE moduleHandle = static_cast<HMODULE>(library.GetHandle());
			const HRSRC resourceHandle = ::FindResourceW(moduleHandle, MAKEINTRESOURCEW(resourceID), resourceType.wc_str());
			return DoLoadResource(resourceHandle, moduleHandle);
		}
	}

	intptr_t ModWindowStyle(void* windowHandle, int index, intptr_t style, bool enable) noexcept
	{
		const HWND handle = reinterpret_cast<HWND>(windowHandle);
		return ::SetWindowLongPtrW(handle, index, ModFlag(::GetWindowLongPtrW(handle, index), style, enable));
	}
}
