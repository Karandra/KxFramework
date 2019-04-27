/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxCOM.h"
#include <combaseapi.h>
#include <unknwn.h>

KxCOMInit::KxCOMInit(tagCOINIT options)
	:m_Result(::CoInitializeEx(nullptr, options))
{
}
KxCOMInit::~KxCOMInit()
{
	Uninitialize();
}

void KxCOMInit::Uninitialize()
{
	if (IsInitialized())
	{
		::CoUninitialize();
		m_Result = GetInvalidHRESULT();
	}
}
