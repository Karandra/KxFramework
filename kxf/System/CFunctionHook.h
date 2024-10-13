#pragma once
#include "Common.h"
#include "Win32Error.h"
#include "SystemThread.h"

namespace kxf
{
	class CFunctionHookController;
	class CFunctionHookTransaction;
}

namespace kxf
{
	class KX_API CFunctionHook
	{
		friend class CFunctionHookTransaction;

		protected:
			String m_Name;
			void* m_OriginalAddress = nullptr;
			void* m_HookAddress = nullptr;
			bool m_Attached = false;

		private:
			void OnAttached(CFunctionHookTransaction& transaction);
			void OnDetached(CFunctionHookTransaction& transaction);

		public:
			CFunctionHook() noexcept = default;
			CFunctionHook(const CFunctionHook&) = delete;
			CFunctionHook(CFunctionHook&& other) noexcept
			{
				*this = std::move(other);
			}
			~CFunctionHook();

		public:
			bool IsNull() const noexcept
			{
				return m_OriginalAddress == nullptr || m_HookAddress == nullptr;
			}
			bool IsAttached() const noexcept
			{
				return m_Attached;
			}

			String GetName() const
			{
				return m_Name;
			}
			void* GetHookAddress() const noexcept
			{
				return m_HookAddress;
			}
			void* GetOriginalAddress() const noexcept
			{
				return m_OriginalAddress;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			CFunctionHook& operator=(const CFunctionHook&) = delete;
			CFunctionHook& operator=(CFunctionHook&& other) noexcept
			{
				m_Name = std::move(other.m_Name);
				m_OriginalAddress = Utility::ExchangeResetAndReturn(other.m_OriginalAddress, nullptr);
				m_HookAddress = Utility::ExchangeResetAndReturn(other.m_HookAddress, nullptr);
				m_Attached = Utility::ExchangeResetAndReturn(other.m_Attached, false);

				return *this;
			}
	};

	template<class TFunc>
	requires(std::is_function_v<TFunc>)
	class CFunctionTypedHook: public CFunctionHook
	{
		public:
			CFunctionTypedHook() noexcept = default;

		public:
			template<class... Args>
			requires(std::is_invocable_v<TFunc, Args...>)
			auto InvokeHook(Args&&... arg)
			{
				return std::invoke(static_cast<TFunc*>(m_HookAddress), std::forward<Args>(arg)...);
			}

			template<class... Args>
			requires(std::is_invocable_v<TFunc, Args...>)
			auto InvokeOriginal(Args&&... arg)
			{
				return std::invoke(static_cast<TFunc*>(m_OriginalAddress), std::forward<Args>(arg)...);
			}
	};
}

namespace kxf
{
	class KX_API CFunctionHookTransaction final
	{
		friend class CFunctionHookController;
		enum class State
		{
			Unknown = -1,

			Wait,
			Ready,
			Aborted,
			Committed
		};

		private:
			CFunctionHookController* m_Controller = nullptr;
			RunningSystemThread m_Thread;
			State m_State = State::Unknown;
			Win32Error m_LastError = Win32Error::Fail();

		private:
			bool DoAttachHook(CFunctionHook& hook, void* originalPtr, void* hookPtr, String name);
			bool DoDetachHook(CFunctionHook& hook);

		private:
			CFunctionHookTransaction(CFunctionHookController& controller, RunningSystemThread thread);

		public:
			CFunctionHookTransaction(const CFunctionHookTransaction&) = delete;
			CFunctionHookTransaction(CFunctionHookTransaction&&) noexcept = default;
			~CFunctionHookTransaction() noexcept;

		public:
			Win32Error GetLastError() const noexcept
			{
				return m_LastError;
			}
			bool IsReady() const noexcept
			{
				return m_LastError.IsSuccess() && m_State == State::Ready;
			}
			bool Commit();
			bool Abort();

			CFunctionHook AttachFunctionPtr(void* address, void* hook, String name = {})
			{
				CFunctionHook result;
				if (DoAttachHook(result, address, hook, std::move(name)))
				{
					return result;
				}
				return {};
			}

			template<class TFunc>
			requires(std::is_function_v<TFunc>)
			CFunctionTypedHook<TFunc> AttachFunction(TFunc* address, TFunc* hook, String name = {})
			{
				CFunctionTypedHook<TFunc> result;
				if (DoAttachHook(result, address, hook, std::move(name)))
				{
					return result;
				}
				return {};
			}

			bool DetachHook(CFunctionHook& hook)
			{
				return DoDetachHook(hook);
			}

		public:
			explicit operator bool() const noexcept
			{
				return IsReady();
			}
			bool operator!() const noexcept
			{
				return !IsReady();
			}

			CFunctionHookTransaction& operator=(const CFunctionHookTransaction&) = delete;
			CFunctionHookTransaction& operator=(CFunctionHookTransaction&&) noexcept = default;
	};
}

namespace kxf
{
	class KX_API CFunctionHookController final
	{
		public:
			static CFunctionHookController& GetInstance()
			{
				static CFunctionHookController instance;
				return instance;
			}

		private:
			Win32Error m_LastError = Win32Error::Success();

		private:
			CFunctionHookController();
			~CFunctionHookController();

		public:
			Win32Error GetLastError() const noexcept
			{
				return m_LastError;
			}

			CFunctionHookTransaction NewTransaction(RunningSystemThread thread = RunningSystemThread::GetCurrentThread());
	};
}
