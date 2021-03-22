#include "KxfPCH.h"
#include "System.h"
#include "kxf/Utility/ScopeGuard.h"
#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::System::Private
{
	String FormatMessage(const void* source, uint32_t messageID, uint32_t flags, const Locale& locale) noexcept
	{
		auto lcid = (locale && !locale.IsInvariant() ? locale : locale.GetUserDefault()).GetLCID();

		wchar_t* formattedMessage = nullptr;
		uint32_t length = ::FormatMessageW(flags|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,
										   source,
										   messageID,
										   lcid.value_or(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT)),
										   reinterpret_cast<wchar_t*>(&formattedMessage),
										   0,
										   nullptr
		);
		if (length != 0 && formattedMessage)
		{
			Utility::ScopeGuard atExit([&]()
			{
				::LocalFree(formattedMessage);
			});
			return String(formattedMessage, length).Trim().Trim(StringOpFlag::FromEnd);
		}
		return {};
	}

	String ResourceTypeToName(size_t id)
	{
		wchar_t buffer[64] = {};
		swprintf_s(buffer, L"%zu", id);

		return buffer;
	}
	String ResourceTypeToName(const wchar_t* id)
	{
		return ResourceTypeToName(reinterpret_cast<size_t>(id));
	}
	const wchar_t* MakeIntResource(int resID)
	{
		return MAKEINTRESOURCEW(resID);
	}

	uint32_t MapSystemProcessAccess(FlagSet<SystemProcessAccess> access) noexcept
	{
		if (access == SystemProcessAccess::Everything)
		{
			return PROCESS_ALL_ACCESS;
		}
		else
		{
			uint32_t nativeAccess = 0;
			Utility::AddFlagRef(nativeAccess, PROCESS_CREATE_PROCESS, access & SystemProcessAccess::CreateProcess);
			Utility::AddFlagRef(nativeAccess, PROCESS_CREATE_THREAD, access & SystemProcessAccess::CreateThread);
			Utility::AddFlagRef(nativeAccess, PROCESS_QUERY_INFORMATION, access & SystemProcessAccess::QueryInformation);
			Utility::AddFlagRef(nativeAccess, PROCESS_QUERY_LIMITED_INFORMATION, access & SystemProcessAccess::QueryLimitedInformation);
			Utility::AddFlagRef(nativeAccess, PROCESS_SET_INFORMATION, access & SystemProcessAccess::SetInformation);
			Utility::AddFlagRef(nativeAccess, PROCESS_SUSPEND_RESUME, access & SystemProcessAccess::SuspendResume);
			Utility::AddFlagRef(nativeAccess, PROCESS_TERMINATE, access & SystemProcessAccess::Terminate);
			Utility::AddFlagRef(nativeAccess, PROCESS_VM_OPERATION, access & SystemProcessAccess::VMOperation);
			Utility::AddFlagRef(nativeAccess, PROCESS_VM_READ, access & SystemProcessAccess::VMRead);
			Utility::AddFlagRef(nativeAccess, PROCESS_VM_WRITE, access & SystemProcessAccess::VMWrite);
			Utility::AddFlagRef(nativeAccess, SYNCHRONIZE, access & SystemProcessAccess::Synchronize);

			return nativeAccess;
		}
	}

	std::optional<uint32_t> MapSystemProcessPriority(SystemProcessPriority priority) noexcept
	{
		switch (priority)
		{
			case SystemProcessPriority::AboveNormal:
			{
				return ABOVE_NORMAL_PRIORITY_CLASS;
			}
			case SystemProcessPriority::BelowNormal:
			{
				return BELOW_NORMAL_PRIORITY_CLASS;
			}
			case SystemProcessPriority::High:
			{
				return HIGH_PRIORITY_CLASS;
			}
			case SystemProcessPriority::Idle:
			{
				return IDLE_PRIORITY_CLASS;
			}
			case SystemProcessPriority::Normal:
			{
				return NORMAL_PRIORITY_CLASS;
			}
			case SystemProcessPriority::Realtime:
			{
				return REALTIME_PRIORITY_CLASS;
			}
		};
		return {};
	}
	SystemProcessPriority MapSystemProcessPriority(uint32_t priority) noexcept
	{
		switch (priority)
		{
			case ABOVE_NORMAL_PRIORITY_CLASS:
			{
				return SystemProcessPriority::AboveNormal;
			}
			case BELOW_NORMAL_PRIORITY_CLASS:
			{
				return SystemProcessPriority::BelowNormal;
			}
			case HIGH_PRIORITY_CLASS:
			{
				return SystemProcessPriority::High;
			}
			case IDLE_PRIORITY_CLASS:
			{
				return SystemProcessPriority::Idle;
			}
			case NORMAL_PRIORITY_CLASS:
			{
				return SystemProcessPriority::Normal;
			}
			case REALTIME_PRIORITY_CLASS:
			{
				return SystemProcessPriority::Realtime;
			}
		};
		return SystemProcessPriority::None;
	}
}
