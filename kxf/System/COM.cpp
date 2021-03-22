#include "KxfPCH.h"
#include "COM.h"
#include "kxf/Utility/Common.h"

#include <Windows.h>
#include <combaseapi.h>
#include <unknwn.h>
#include "UndefWindows.h"

namespace
{
	using namespace kxf;

	constexpr DWORD MapCOMInitFlag(FlagSet<COMInitFlag> flags) noexcept
	{
		DWORD nativeFlags = 0;
		Utility::AddFlagRef(nativeFlags, COINIT::COINIT_DISABLE_OLE1DDE, flags & COMInitFlag::DisableOLE1DDE);
		Utility::AddFlagRef(nativeFlags, COINIT::COINIT_SPEED_OVER_MEMORY, flags & COMInitFlag::SppedOverMemory);

		return nativeFlags;
	}
	constexpr CLSCTX MapClassContext(FlagSet<ClassContext> context) noexcept
	{
		CLSCTX nativeContext = static_cast<CLSCTX>(0);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_INPROC_SERVER, context & ClassContext::InprocServer);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_INPROC_HANDLER, context & ClassContext::InprocHandler);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_LOCAL_SERVER, context & ClassContext::LocalServer);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_REMOTE_SERVER, context & ClassContext::RemoteServer);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_ENABLE_CODE_DOWNLOAD, context & ClassContext::EnableCodeDownload);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_NO_CODE_DOWNLOAD, context & ClassContext::NoCodeDownload);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_NO_CUSTOM_MARSHAL, context & ClassContext::NoCustomMarshal);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_NO_FAILURE_LOG, context & ClassContext::NoFailureLog);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_DISABLE_AAA, context & ClassContext::DisableAAA);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_ENABLE_AAA, context & ClassContext::EnableAAA);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_ACTIVATE_AAA_AS_IU, context & ClassContext::ActivateAAAAsIU);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_FROM_DEFAULT_CONTEXT, context & ClassContext::FromDefaultContext);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_ACTIVATE_32_BIT_SERVER, context & ClassContext::Activate32BitServer);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_ACTIVATE_64_BIT_SERVER, context & ClassContext::Activate64BitServer);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_ACTIVATE_ARM32_SERVER, context & ClassContext::ActivateARM32Server);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_ENABLE_CLOAKING, context & ClassContext::EnableCloaking);
		Utility::AddFlagRef(nativeContext, CLSCTX::CLSCTX_APPCONTAINER, context & ClassContext::AppContainer);

		return nativeContext;
	}
}

namespace kxf::COM
{
	void* AllocateMemory(size_t size) noexcept
	{
		return ::CoTaskMemAlloc(size);
	}
	void* ReallocateMemory(void* address, size_t size) noexcept
	{
		return ::CoTaskMemRealloc(address, size);
	}
	void FreeMemory(void* address) noexcept
	{
		::CoTaskMemFree(address);
	}

	wchar_t* AllocateBSTR(const wchar_t* data) noexcept
	{
		return ::SysAllocString(data);
	}
	void FreeBSTR(wchar_t* data) noexcept
	{
		::SysFreeString(data);
	}

	::_GUID ToGUID(const NativeUUID& uuid) noexcept
	{
		::GUID guid = {};
		guid.Data1 = uuid.Data1;
		guid.Data2 = uuid.Data2;
		guid.Data3 = uuid.Data3;
		for (size_t i = 0; i < std::size(guid.Data4); i++)
		{
			guid.Data4[i] = uuid.Data4[i];
		}

		return guid;
	}
	NativeUUID FromGUID(const ::_GUID& guid) noexcept
	{
		NativeUUID uuid;
		uuid.Data1 = guid.Data1;
		uuid.Data2 = guid.Data2;
		uuid.Data3 = guid.Data3;
		for (size_t i = 0; i < std::size(uuid.Data4); i++)
		{
			uuid.Data4[i] = guid.Data4[i];
		}

		return uuid;
	}

	HResult CreateInstance(const NativeUUID& classID, ClassContext classContext, const NativeUUID& iid, void** result, IUnknown* outer) noexcept
	{
		return ::CoCreateInstance(ToGUID(classID), outer, MapClassContext(classContext), ToGUID(iid), result);
	}
}

namespace kxf
{
	void COMInitGuard::DoInitialize(COMThreadingModel threadingModel, FlagSet<COMInitFlag> flags) noexcept
	{
		switch (threadingModel)
		{
			case COMThreadingModel::Apartment:
			{
				m_Status = ::CoInitializeEx(nullptr, COINIT::COINIT_APARTMENTTHREADED|MapCOMInitFlag(flags));
			}
			case COMThreadingModel::Concurrent:
			{
				m_Status = ::CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED|MapCOMInitFlag(flags));
			}
		};
	}
	void COMInitGuard::DoUninitialize() noexcept
	{
		if (IsInitialized())
		{
			::CoUninitialize();
			m_Status = HResult::Pending();
		}
	}
}

namespace kxf
{
	void OLEInitGuard::DoInitialize() noexcept
	{
		m_Status = ::OleInitialize(nullptr);
	}
	void OLEInitGuard::DoUninitialize() noexcept
	{
		if (IsInitialized())
		{
			::OleUninitialize();
			m_Status = HResult::Pending();
		}
	}
}
