#include "KxStdAfx.h"
#include "COM.h"
#include "Kx/Utility/Common.h"
#include <combaseapi.h>
#include <unknwn.h>

namespace
{
	constexpr DWORD MapCOMInitFlag(KxFramework::COMInitFlag flags) noexcept
	{
		using namespace KxFramework;

		DWORD nativeFlags = 0;
		Utility::AddFlagRef(nativeFlags, COINIT::COINIT_DISABLE_OLE1DDE, flags & COMInitFlag::DisableOLE1DDE);
		Utility::AddFlagRef(nativeFlags, COINIT::COINIT_SPEED_OVER_MEMORY, flags & COMInitFlag::SppedOverMemory);

		return nativeFlags;
	}
}

namespace KxFramework
{
	void COMInitGuard::DoInitialize(COMThreadingModel threadingModel, COMInitFlag flags) noexcept
	{
		switch (threadingModel)
		{
			case COMThreadingModel::Apartment:
			{
				m_Status = HResult(::CoInitializeEx(nullptr, COINIT::COINIT_APARTMENTTHREADED|MapCOMInitFlag(flags)));
			}
			case COMThreadingModel::Concurrent:
			{
				m_Status = HResult(::CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED|MapCOMInitFlag(flags)));
			}
		};
	}
	void COMInitGuard::DoUninitialize() noexcept
	{
		if (IsInitialized())
		{
			::CoUninitialize();
			m_Status = {};
		}
	}
}

namespace KxFramework
{
	void OLEInitGuard::DoInitialize() noexcept
	{
		m_Status = HResult(::OleInitialize(nullptr));
	}
	void OLEInitGuard::DoUninitialize() noexcept
	{
		if (IsInitialized())
		{
			::OleUninitialize();
			m_Status = {};
		}
	}
}
