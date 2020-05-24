#pragma once
#include "Common.h"

namespace kxf::Utility
{
	template<class TFunc>
	class KX_API CallAtScopeExit final
	{
		private:
			TFunc m_Func;

		public:
			CallAtScopeExit(TFunc&& func) noexcept
				:m_Func(std::forward<TFunc>(func))
			{
			}
			~CallAtScopeExit() noexcept
			{
				std::invoke(m_Func);
			}
	};
}
