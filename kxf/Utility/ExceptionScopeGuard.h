#pragma once
#include "Common.h"
#include "ScopeGuard.h"
#include <exception>

namespace kxf::Utility
{
	template<class TFunc_>
	class ExceptionScopeGuard final
	{
		static_assert(std::is_nothrow_invocable_v<TFunc_>, "no-throw invokable function is required");

		public:
			using TFunc = TFunc_;

		private:
			ScopeGuard<TFunc> m_Guard;
			int m_UncaughtExceptions = std::uncaught_exceptions();

		public:
			ExceptionScopeGuard(TFunc&& func) noexcept
				:m_Guard(std::forward<TFunc>(func))
			{
			}
			ExceptionScopeGuard(ExceptionScopeGuard&& other) noexcept
			{
				*this = std::move(other);
			}
			ExceptionScopeGuard(const ExceptionScopeGuard&) = delete;
			~ExceptionScopeGuard() noexcept
			{
				if (m_UncaughtExceptions != std::uncaught_exceptions())
				{
					m_Guard.Invoke();
				}
				else
				{
					m_Guard.Dismiss();
				}
			}

		public:
			bool IsActive() const noexcept
			{
				return m_Guard.IsActive();
			}
			void Dismiss() noexcept
			{
				m_Guard.Dismiss();
			}
			void Invoke()
			{
				m_Guard.Invoke();
			}

		public:
			ExceptionScopeGuard& operator=(ExceptionScopeGuard&& other) noexcept
			{
				m_Guard = std::move(other.m_Guard);

				m_UncaughtExceptions = other.m_UncaughtExceptions;
				other.m_UncaughtExceptions = std::numeric_limits<int>::max();

				return *this;
			}
			ExceptionScopeGuard& operator=(const ExceptionScopeGuard&) = delete;
	};
}
