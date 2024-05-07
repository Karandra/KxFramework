#pragma once
#include "ISystemWindow.h"
#include "../SystemProcess/RunningSystemProcess.h"

namespace kxf
{
	class KX_API SystemWindow: public ISystemWindow
	{
		public:
			static SystemWindow GetShellWindow();
			static SystemWindow GetDesktopWindow();
			static SystemWindow GetBroadcastWindow();
			static SystemWindow GetForegroundWindow();
			static SystemWindow GetWindowFromPoint(const Point& position);
			static SystemWindow GetWindowFromPhysicalPoint(const Point& position);

		private:
			void* m_Handle = nullptr;
			bool m_IsOwned = false;

		public:
			SystemWindow() = default;
			SystemWindow(void* hwnd, bool ownHndle = false)
				:m_Handle(hwnd), m_IsOwned(ownHndle)
			{
			}
			SystemWindow(const SystemWindow&) = delete;
			SystemWindow(SystemWindow&& other) noexcept
			{
				*this = std::move(other);
			}
			~SystemWindow()
			{
				if (m_IsOwned)
				{
					Destroy();
				}
			}

		public:
			// ISystemThread
			bool IsNull() const override
			{
				return m_Handle == nullptr;
			}
			uint32_t GetID() const override;
			void* GetHandle() const override
			{
				return m_Handle;
			}
			SystemThread GetOwningThread() const override;
			SystemProcess GetOwningProcess() const override;

			String GetText() const override;
			bool SetText(const String& label) override;

			Rect GetRect() const override;
			Point ScreenToClient(const Point& point) const override;
			Point ClientToScreen(const Point& point) const override;
			Size MapPoints(Rect& rect, const ISystemWindow& targetWindow) const override;

			std::optional<intptr_t> GetValue(int index) const override;
			std::optional<intptr_t> SetValue(int index, intptr_t value) override;

			bool PostMessage(uint32_t message, intptr_t wParam, intptr_t lParam) override;
			std::optional<intptr_t> SendMessage(uint32_t message, intptr_t wParam, intptr_t lParam, FlagSet<uint32_t> flags = {}, TimeSpan timeout = {}) override;

			bool Show(SHWindowCommand command, bool async) override;
			bool Close() override;
			bool Destroy() override;

			// SystemWindow
			RunningSystemProcess OpenOwningProcess() const;

			bool IsBoradcastWindow() const;
			bool AttachHandle(void* hwnd, bool own = false)
			{
				if (!m_Handle)
				{
					m_Handle = hwnd;
					m_IsOwned = own;
					return true;
				}
				return false;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle;
				m_Handle = nullptr;
				m_IsOwned = false;

				return handle;
			}

			template<class TFunc>
			requires(std::is_convertible_v<std::invoke_result_t<TFunc, FlagSet<intptr_t>>, FlagSet<intptr_t>>)
			std::optional<intptr_t> ModWindowStyle(int index, TFunc&& func)
			{
				return GetValue(index).and_then([&](intptr_t value)
				{
					FlagSet<intptr_t> flags = std::invoke(func, FlagSet(value));
					return SetValue(index, *flags);
				});
			}

			std::optional<intptr_t> ModWindowStyle(int index, FlagSet<intptr_t> style, bool enable)
			{
				return ModWindowStyle(index, [&](auto flags)
				{
					return flags.Mod(style, enable);
				});
			}

		public:
			SystemWindow& operator=(const SystemWindow&) = delete;
			SystemWindow& operator=(SystemWindow&& other) noexcept
			{
				m_IsOwned = other.m_IsOwned;
				m_Handle = other.m_Handle;

				other.m_Handle = nullptr;
				other.m_IsOwned = false;

				return *this;
			}
	};
}
