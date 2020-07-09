#include "stdafx.h"
#include "NativeApp.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/DynamicLibraryEvent.h"

namespace
{
	enum
	{
		LDR_DLL_NOTIFICATION_REASON_LOADED = 1,
		LDR_DLL_NOTIFICATION_REASON_UNLOADED = 2
	};

	struct UNICODE_STRING final
	{
		USHORT Length = 0;
		USHORT MaximumLength = 0;
		PWCH Buffer = nullptr;
	};
	struct LDR_DLL_LOADED_NOTIFICATION_DATA final
	{
		ULONG Flags = 0;
		const UNICODE_STRING* FullDllName = nullptr;
		const UNICODE_STRING* BaseDllName = nullptr;
		void* DllBase = nullptr;
		ULONG SizeOfImage = 0;
	};
	struct LDR_DLL_UNLOADED_NOTIFICATION_DATA final
	{
		ULONG Flags = 0;
		const UNICODE_STRING* FullDllName = nullptr;
		const UNICODE_STRING* BaseDllName = nullptr;
		void* DllBase = nullptr;
		ULONG SizeOfImage = 0;
	};
	union LDR_DLL_NOTIFICATION_DATA final
	{
		LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
		LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
	};

	kxf::String FromUNICODE_STRING(const UNICODE_STRING& uniString)
	{
		return kxf::String(uniString.Buffer, uniString.Length / sizeof(*UNICODE_STRING::Buffer));
	}
}

namespace kxf::Private
{
	void NativeApp::OnCreate()
	{
		// Nothing to do here yet
	}
	void NativeApp::OnDestroy()
	{
		if (NativeAPI::NtDLL::LdrUnregisterDllNotification && m_DLLNotificationsCookie)
		{
			NativeAPI::NtDLL::LdrUnregisterDllNotification(m_DLLNotificationsCookie);
		}
		m_DLLNotificationsCookie = nullptr;
	}

	bool NativeApp::OnBindDLLNotification()
	{
		if (!NativeAPI::NtDLL::LdrRegisterDllNotification)
		{
			return false;
		}
		if (!m_DLLNotificationsCookie)
		{
			NtStatus status = NativeAPI::NtDLL::LdrRegisterDllNotification(0, [](uint32_t reason, const void* data, void* context)
			{
				DynamicLibraryEvent event;
				auto SetParameters = [&](const auto& parameters)
				{
					event.SetLibrary(parameters.DllBase);
					if (parameters.BaseDllName)
					{
						event.SetBaseName(FromUNICODE_STRING(*parameters.BaseDllName));
					}
					if (parameters.FullDllName)
					{
						event.SetFullPath(FromUNICODE_STRING(*parameters.FullDllName));
					}
				};

				switch (reason)
				{
					case LDR_DLL_NOTIFICATION_REASON_LOADED:
					{
						event.SetEventType(DynamicLibraryEvent::EvtLoaded);
						SetParameters(reinterpret_cast<const LDR_DLL_NOTIFICATION_DATA*>(data)->Loaded);
						break;
					}
					case LDR_DLL_NOTIFICATION_REASON_UNLOADED:
					{
						event.SetEventType(DynamicLibraryEvent::EvtUnloaded);
						SetParameters(reinterpret_cast<const LDR_DLL_NOTIFICATION_DATA*>(data)->Unloaded);
						break;
					}
				};

				if (event.GetEventType() != Event::EvtNull)
				{
					reinterpret_cast<NativeApp*>(context)->ProcessEvent(event);
				}
			}, this, &m_DLLNotificationsCookie);
			return status.IsSuccess() && m_DLLNotificationsCookie;
		}
		return true;
	}
	bool NativeApp::OnDynamicBind(wxDynamicEventTableEntry& entry)
	{
		if (wxApp::OnDynamicBind(entry))
		{
			if (entry.m_eventType == DynamicLibraryEvent::EvtLoaded || entry.m_eventType == DynamicLibraryEvent::EvtUnloaded)
			{
				return OnBindDLLNotification();
			}
		}
		return false;
	}
}
