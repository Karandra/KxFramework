#include "KxfPCH.h"
#include "Mutex.h"
#include "kxf/System/Win32Error.h"
#include <Windows.h>

namespace
{
	kxf::String GetFullName(const kxf::String& name, bool isGlobal)
	{
		if (!name.IsEmpty())
		{
			return kxf::String::Format(wxS("%1\\%2"), isGlobal ? "Global" : "Local", name);
		}
		return {};
	}
}

namespace kxf::Private
{
	bool BasicMutex::Create(const String& name, bool acquireInitially, bool isGlobal) noexcept
	{
		if (!name.IsEmpty())
		{
			String fullName = GetFullName(name, isGlobal);
			m_Handle = ::CreateMutexW(nullptr, acquireInitially, fullName.wc_str());
		}
		else
		{
			m_Handle = ::CreateMutexW(nullptr, acquireInitially, nullptr);
		}
		return m_Handle != nullptr;
	}
	bool BasicMutex::Open(const String& name, bool isGlobal) noexcept
	{
		String fullName = GetFullName(name, isGlobal);
		m_Handle = ::OpenMutexW(SYNCHRONIZE, FALSE, fullName.wc_str());

		return m_Handle != nullptr;
	}

	void BasicMutex::Destroy() noexcept
	{
		if (m_Handle)
		{
			::CloseHandle(m_Handle);
			m_Handle = nullptr;
		}
	}
	bool BasicMutex::Acquire(const TimeSpan& timeout) noexcept
	{
		return ::WaitForSingleObject(m_Handle, timeout.IsPositive() ? timeout.GetMilliseconds() : INFINITE) == WAIT_OBJECT_0;
	}
	bool BasicMutex::Release() noexcept
	{
		return ::ReleaseMutex(m_Handle);
	}
}
