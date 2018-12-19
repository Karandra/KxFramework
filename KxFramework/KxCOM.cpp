/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxCOM.h"
#include <combaseapi.h>
#include <unknwn.h>

KxCOMInit::KxCOMInit(DWORD options)
	:m_Result(E_FAIL)
{
	m_Result = ::CoInitializeEx(nullptr, options);
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
