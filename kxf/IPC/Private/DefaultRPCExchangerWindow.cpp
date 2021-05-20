#include "KxfPCH.h"
#include "DefaultRPCExchangerWindow.h"
#include "DefaultRPCExchanger.h"
#include "kxf/System/Win32Error.h"
#include "kxf/IO/MemoryStream.h"
#include <Windows.h>

namespace kxf
{
	bool DefaultRPCExchangerWindow::MSWHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if (msg == WM_COPYDATA)
		{
			const COPYDATASTRUCT* copyData = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
			MemoryInputStream stream(copyData->lpData, copyData->cbData);
			m_Exchanger.OnDataRecieved(stream);

			*result = TRUE;
			return true;
		}
		return UI::Frame::MSWHandleMessage(result, msg, wParam, lParam);
	}

	DefaultRPCExchangerWindow::DefaultRPCExchangerWindow(DefaultRPCExchanger& exchanger, const UniversallyUniqueID& sessionID)
		:Frame(nullptr, wxID_NONE, exchanger.GetControlBufferName()), m_Exchanger(exchanger)
	{
	}
}
