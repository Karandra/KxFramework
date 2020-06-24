#include "stdafx.h"
#include "PasswordHandler.h"
#include "Utility.h"

#include "kxf/System/Private/BeginIncludeCOM.h"
namespace
{
	Kx_MakeWinUnicodeCallWrapper(FormatMessage);
}
#include <comdef.h>
#include <comutil.h>
#include "kxf/System/Private/EndIncludeCOM.h"

namespace kxf::SevenZip::Private
{
	HResult PasswordHandler::OnPasswordRequest(BSTR* password, Int32* passwordIsDefined)
	{
		if (!password)
		{
			return E_POINTER;
		}
		if (m_EvtHandler)
		{
			ArchiveEvent event = CreateEvent(ArchiveEvent::EvtGetPassword);
			if (SendEvent(event))
			{
				String value = std::move(event).GetPassword().ToString();
				*password = _bstr_t(value.wx_str()).Detach();
				Utility::SetIfNotNull(passwordIsDefined, 1);

				return S_OK;
			}
		}
		return E_ABORT;
	}
}
