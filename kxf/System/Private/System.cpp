#include "KxfPCH.h"
#include "System.h"
#include "kxf/Core/Format.h"
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
			return String(formattedMessage, length).Trim().Trim(StringActionFlag::FromEnd);
		}
		return {};
	}

	String ResourceTypeToName(size_t id)
	{
		return ToString(id);
	}
	String ResourceTypeToName(const wchar_t* id)
	{
		return ResourceTypeToName(reinterpret_cast<size_t>(id));
	}
	const wchar_t* MakeIntResource(int resID)
	{
		return MAKEINTRESOURCEW(resID);
	}

	FlagSet<uint32_t> MapSystemProcessAccess(FlagSet<SystemProcessAccess> access) noexcept
	{
		if (access == SystemProcessAccess::Everything)
		{
			return PROCESS_ALL_ACCESS;
		}
		else
		{
			FlagSet<uint32_t> nativeAccess;
			nativeAccess.Add(PROCESS_CREATE_PROCESS, access & SystemProcessAccess::CreateProcess);
			nativeAccess.Add(PROCESS_CREATE_THREAD, access & SystemProcessAccess::CreateThread);
			nativeAccess.Add(PROCESS_QUERY_INFORMATION, access & SystemProcessAccess::QueryInformation);
			nativeAccess.Add(PROCESS_QUERY_LIMITED_INFORMATION, access & SystemProcessAccess::QueryLimitedInformation);
			nativeAccess.Add(PROCESS_SET_INFORMATION, access & SystemProcessAccess::SetInformation);
			nativeAccess.Add(PROCESS_SUSPEND_RESUME, access & SystemProcessAccess::SuspendResume);
			nativeAccess.Add(PROCESS_TERMINATE, access & SystemProcessAccess::Terminate);
			nativeAccess.Add(PROCESS_VM_OPERATION, access & SystemProcessAccess::VMOperation);
			nativeAccess.Add(PROCESS_VM_READ, access & SystemProcessAccess::VMRead);
			nativeAccess.Add(PROCESS_VM_WRITE, access & SystemProcessAccess::VMWrite);
			nativeAccess.Add(SYNCHRONIZE, access & SystemProcessAccess::Synchronize);

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

	String GetKernelObjectName(const String& name, KernelObjectNamespace ns)
	{
		if (!name.IsEmpty())
		{
			switch (ns)
			{
				case KernelObjectNamespace::Local:
				{
					return Format("Local\\{}", name);
				}
				case KernelObjectNamespace::Global:
				{
					return Format("Global\\{}", name);
				}
			};
		}
		return {};
	}
}
