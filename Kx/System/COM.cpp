#include "KxStdAfx.h"
#include "COM.h"
#include <combaseapi.h>
#include <unknwn.h>

namespace KxFramework
{
	void COMInitGuard::DoInitialize(COMThreadingModel threadingModel)
	{
		switch (threadingModel)
		{
			case COMThreadingModel::Apartment:
			{
				m_Status = ErrorCode::FromHRESULT(::CoInitializeEx(nullptr, COINIT::COINIT_APARTMENTTHREADED));
			}
			case COMThreadingModel::Concurrent:
			{
				m_Status = ErrorCode::FromHRESULT(::CoInitializeEx(nullptr, COINIT::COINIT_MULTITHREADED));
			}
		};
	}
	void COMInitGuard::DoUninitialize()
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
	void OLEInitGuard::DoInitialize()
	{
		m_Status = ErrorCode::FromHRESULT(::OleInitialize(nullptr));
	}
	void OLEInitGuard::DoUninitialize()
	{
		if (IsInitialized())
		{
			::OleUninitialize();
			m_Status = {};
		}
	}
}
