#include "KxStdAfx.h"
#include "KxFramework/KxCOM.h"
#include <combaseapi.h>
#include <unknwn.h>

KxCOMInit::KxCOMInit(DWORD options)
	:m_Result(E_FAIL)
{
	m_Result = ::CoInitializeEx(NULL, options);
	m_IsInit = SUCCEEDED(m_Result);
}
KxCOMInit::~KxCOMInit()
{
	Uninitialize();
}

void KxCOMInit::Uninitialize()
{
	if (m_IsInit)
	{
		::CoUninitialize();
		m_IsInit = false;
	}
}
