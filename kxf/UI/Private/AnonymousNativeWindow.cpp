#include "KxfPCH.h"
#include "AnonymousNativeWindow.h"
#include "kxf/Log/Common.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/System/Win32Error.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/IO/MemoryStream.h"
#include <Windows.h>

namespace
{
	constexpr wchar_t g_WindowClassName[] = L"kxf::AnonymousNativeWindow";

	HMODULE GetCurrentModule() noexcept
	{
		return reinterpret_cast<HMODULE>(kxf::DynamicLibrary::GetCurrentModule().GetHandle());
	}
}

namespace kxf::Private
{
	bool AnonymousNativeWindow::HandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam)
	{
		if (m_HandleMessage)
		{
			try
			{
				return std::invoke(m_HandleMessage, result, msg, wParam, lParam);
			}
			catch (...)
			{
				if (auto app = ICoreApplication::GetInstance())
				{
					app->StoreCurrentException();
					app->OnUnhandledException();

					std::terminate();
				}
				else
				{
					Log::FatalError(__FUNCTION__ ": Unhandled exception occurred");
					throw;
				}
			}
		}
		return false;
	}

	bool AnonymousNativeWindow::Create(decltype(m_HandleMessage) messageHandler, const String& title)
	{
		if (!m_Handle)
		{
			m_HandleMessage = std::move(messageHandler);

			WNDCLASSEXW windowClass = {};
			windowClass.cbSize = sizeof(windowClass);
			windowClass.lpszClassName = g_WindowClassName;
			windowClass.hInstance = GetCurrentModule();
			windowClass.lpfnWndProc = [](HWND handle, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
			{
				if (auto window = reinterpret_cast<AnonymousNativeWindow*>(::GetWindowLongPtrW(handle, GWLP_USERDATA)))
				{
					intptr_t result = FALSE;
					if (window->HandleMessage(result, msg, wParam, lParam))
					{
						return result;
					}
				}
				return ::DefWindowProcW(handle, msg, wParam, lParam);
			};
			m_WindowClass = ::RegisterClassExW(&windowClass);

			if (m_WindowClass != 0)
			{
				const wchar_t* windowTitle = !title.IsEmpty() ? title.wc_str() : windowClass.lpszClassName;
				if (m_Handle = ::CreateWindowExW(0, windowClass.lpszClassName, windowTitle, 0, -1, -1, 0, 0, nullptr, nullptr, windowClass.hInstance, nullptr))
				{
					::SetWindowLongPtrW(reinterpret_cast<HWND>(m_Handle), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
					return true;
				}
			}
		}
		return false;
	}
	bool AnonymousNativeWindow::Destroy() noexcept
	{
		bool destroyed = false;
		if (m_Handle)
		{
			destroyed = ::DestroyWindow(reinterpret_cast<HWND>(m_Handle));
			m_Handle = nullptr;
		}
		if (m_WindowClass != 0)
		{
			::UnregisterClassW(g_WindowClassName, GetCurrentModule());
			m_WindowClass = 0;
		}
		return false;
	}
}
