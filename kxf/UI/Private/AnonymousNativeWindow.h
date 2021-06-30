#pragma once
#include "Common.h"
class wxWindow;
class wxNativeWindow;

namespace kxf::Private
{
	class AnonymousNativeWindow final
	{
		private:
			std::function<bool(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam)> m_HandleMessage;
			void* m_Handle = nullptr;
			uint32_t m_WindowClass = 0;

			std::unique_ptr<wxNativeWindow> m_NativeWindow;

		private:
			bool HandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam) noexcept;

		public:
			AnonymousNativeWindow() noexcept;
			AnonymousNativeWindow(const AnonymousNativeWindow&) = delete;
			~AnonymousNativeWindow() noexcept;

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			void* GetHandle() const noexcept
			{
				return m_Handle;
			}
			wxWindow* GetWxWindow();

			bool Create(decltype(m_HandleMessage) messageHandler, const String& title = {});
			bool Destroy() noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			AnonymousNativeWindow& operator=(const AnonymousNativeWindow&) = delete;
	};
}
