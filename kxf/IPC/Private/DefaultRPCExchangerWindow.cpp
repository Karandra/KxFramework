#include "KxfPCH.h"
#include "DefaultRPCExchangerWindow.h"
#include "DefaultRPCExchanger.h"
#include "kxf/System/Win32Error.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/IO/MemoryStream.h"
#include <Windows.h>

namespace
{
	constexpr wchar_t g_WindowClassName[] = L"kxf::DefaultRPCExchangerWindow";

	HMODULE GetCurrentModule() noexcept
	{
		return reinterpret_cast<HMODULE>(kxf::DynamicLibrary::GetCurrentModule().GetHandle());
	}
}

namespace kxf
{
	bool DefaultRPCExchangerWindow::MSWHandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam)
	{
		if (msg == WM_COPYDATA)
		{
			const COPYDATASTRUCT* copyData = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
			MemoryInputStream stream(copyData->lpData, copyData->cbData);
			m_Exchanger.OnDataRecieved(stream);

			result = TRUE;
			return true;
		}
		return false;
	}

	bool DefaultRPCExchangerWindow::Create(const UniversallyUniqueID& sessionID)
	{
		if (!m_Handle)
		{
			WNDCLASSEXW windowClass = {};
			windowClass.cbSize = sizeof(windowClass);
			windowClass.lpszClassName = g_WindowClassName;
			windowClass.hInstance = GetCurrentModule();
			windowClass.lpfnWndProc = [](HWND handle, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				if (auto window = reinterpret_cast<DefaultRPCExchangerWindow*>(::GetWindowLongPtrW(handle, GWLP_USERDATA)))
				{
					intptr_t result = FALSE;
					if (window->MSWHandleMessage(result, msg, wParam, lParam))
					{
						return result;
					}
				}
				return ::DefWindowProcW(handle, msg, wParam, lParam);
			};
			m_WindowClass = ::RegisterClassExW(&windowClass);

			if (m_WindowClass != 0)
			{
				String windowName = sessionID.ToString();
				if (m_Handle = ::CreateWindowExW(0, windowClass.lpszClassName, windowName.wc_str(), 0, -1, -1, 0, 0, nullptr, nullptr, windowClass.hInstance, nullptr))
				{
					::SetWindowLongPtrW(reinterpret_cast<HWND>(m_Handle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
					return true;
				}
			}
		}
		return false;
	}
	bool DefaultRPCExchangerWindow::Destroy() noexcept
	{
		if (m_Handle)
		{
			bool destroyed = ::DestroyWindow(reinterpret_cast<HWND>(m_Handle));
			m_Handle = nullptr;

			return destroyed;
		}
		if (m_WindowClass != 0)
		{
			::UnregisterClassW(g_WindowClassName, GetCurrentModule());
			m_WindowClass = 0;
		}
		return false;
	}
}
