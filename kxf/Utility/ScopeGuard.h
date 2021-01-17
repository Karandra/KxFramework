#pragma once
#include "Common.h"
#include <optional>

namespace kxf::Utility
{
	template<class TFunc_>
	class ScopeGuard final
	{
		public:
			using TFunc = TFunc_;

		private:
			std::optional<TFunc> m_Func;

		public:
			ScopeGuard(TFunc&& func) noexcept
				:m_Func(std::forward<TFunc>(func))
			{
			}
			ScopeGuard(ScopeGuard&&) noexcept = default;
			ScopeGuard(const ScopeGuard&) = delete;
			~ScopeGuard() noexcept
			{
				Invoke();
			}

		public:
			bool IsActive() const noexcept
			{
				return m_Func.has_value();
			}
			void Dismiss() noexcept
			{
				m_Func.reset();
			}
			void Invoke()
			{
				if (m_Func)
				{
					std::invoke(*m_Func);
					m_Func.reset();
				}
			}

		public:
			ScopeGuard& operator=(ScopeGuard&&) noexcept = default;
			ScopeGuard& operator=(const ScopeGuard&) = delete;
	};
}
