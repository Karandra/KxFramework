#include "KxStdAfx.h"
#include "System.h"
#include "Common.h"

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

namespace KxFramework::Utility
{
	const wxScopedCharBuffer LoadResource(const String& resourceName, const String& resourceType)
	{
		HMODULE moduleHandle = ::GetModuleHandleW(nullptr);
		HRSRC resourceHandle = ::FindResourceW(moduleHandle, resourceName.wc_str(), resourceType.wc_str());
		return DoLoadResource(resourceHandle, moduleHandle);
	}
	const wxScopedCharBuffer LoadResource(int resourceID, const String& resourceType)
	{
		HMODULE moduleHandle = ::GetModuleHandleW(nullptr);
		HRSRC resourceHandle = ::FindResourceW(moduleHandle, MAKEINTRESOURCEW(resourceID), resourceType.wc_str());
		return DoLoadResource(resourceHandle, moduleHandle);
	}

	intptr_t ModWindowStyle(void* windowHandle, int index, intptr_t style, bool enable) noexcept
	{
		const HWND handle = reinterpret_cast<HWND>(windowHandle);
		::SetWindowLongPtrW(handle, index, ModFlag(::GetWindowLongPtrW(handle, index), style, enable));
	}
}
