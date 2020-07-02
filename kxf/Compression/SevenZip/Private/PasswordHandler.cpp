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
			return HResult::InvalidPointer();
		}
		if (m_EvtHandler)
		{
			ArchiveEvent event = CreateEvent(ArchiveEvent::EvtGetPassword);
			if (SendEvent(event))
			{
				String value = std::move(event).GetPassword().ToString();
				if (!value.IsEmpty())
				{
					*password = _bstr_t(value.wx_str()).Detach();
					Utility::SetIfNotNull(passwordIsDefined, 1);

					return HResult::Success();
				}
				else
				{
					*password = nullptr;
					Utility::SetIfNotNull(passwordIsDefined, 0);

					return HResult::False();
				}
			}
		}
		return HResult::Abort();
	}
}
