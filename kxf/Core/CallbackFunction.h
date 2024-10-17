#pragma once
#include "Common.h"

namespace kxf
{
	enum class CallbackCommand: int32_t
	{
		None = -1,

		Continue,
		Terminate,
		Discard
	};
}

namespace kxf
{
	class KX_API CallbackResult final
	{
		private:
			std::array<size_t, 3> m_Counters;
			CallbackCommand m_LastCommand = CallbackCommand::None;

		public:
			CallbackResult() noexcept
			{
				m_Counters.fill(0);
			}

		public:
			void UpdateWith(CallbackCommand command)
			{
				if (auto index = ToInt(command); index >= 0 && index < m_Counters.size())
				{
					m_Counters[static_cast<size_t>(index)]++;
					m_LastCommand = command;
				}
				else
				{
					m_LastCommand = CallbackCommand::Terminate;
				}
			}

			CallbackCommand GetLastCommand() const noexcept
			{
				return m_LastCommand;
			}
			size_t GetInvokeCount(CallbackCommand command) const noexcept
			{
				if (auto index = ToInt(command); index >= 0 && index < m_Counters.size())
				{
					return m_Counters[static_cast<size_t>(index)];
				}
				return 0;
			}
			size_t GetTotalInvokeCount(CallbackCommand command) const noexcept
			{
				return std::accumulate(m_Counters.begin(), m_Counters.end(), static_cast<size_t>(0));
			}
	};
}

namespace kxf
{
	template<class... Args_>
	class CallbackFunction final
	{
		private:
			std::move_only_function<CallbackCommand(Args_...)> m_Callable;
			CallbackResult m_Result;

		public:
			CallbackFunction() noexcept = default;

			template<class TFunc>
			requires(std::is_same_v<std::invoke_result_t<TFunc, Args_...>, CallbackCommand>)
			CallbackFunction(TFunc&& func)
				:m_Callable(std::move(func))
			{
			}

			template<class TFunc>
			requires(std::is_same_v<std::invoke_result_t<TFunc, Args_...>, void>)
			CallbackFunction(TFunc&& func)
			{
				m_Callable = [callable = std::move(func)](Args_&&... arg)
				{
					std::invoke(callable, std::forward<Args_>(arg)...);
					return CallbackCommand::Continue;
				};
			}

			CallbackFunction(CallbackFunction&&) noexcept = default;
			CallbackFunction(const CallbackFunction&) = delete;

		public:
			bool IsNull() const noexcept
			{
				return !static_cast<bool>(m_Callable);
			}
			void Reset() noexcept
			{
				m_Result = {};
			}
			bool ShouldTerminate() const noexcept
			{
				return m_Result.GetLastCommand() == CallbackCommand::Terminate;
			}

			CallbackFunction& Invoke(Args_... arg)
			{
				if (m_Callable)
				{
					CallbackCommand command = std::invoke(m_Callable, std::forward<Args_>(arg)...);
					m_Result.UpdateWith(command);
				}
				else
				{
					m_Result.UpdateWith(CallbackCommand::Discard);
				}
				return *this;
			}
			CallbackCommand GetLastCommand() const noexcept
			{
				return m_Result.GetLastCommand();
			}
			CallbackResult GetResult() const
			{
				return m_Result;
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

			CallbackFunction& operator=(CallbackFunction&&) noexcept = default;
			CallbackFunction& operator=(const CallbackFunction&) = delete;
	};
};
