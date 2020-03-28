#pragma once
#include "Common.h"

namespace KxFramework::Utility
{
	template<class TFunc>
	class KX_API CallAtScopeExit final
	{
		private:
			TFunc m_Func;

		public:
			CallAtScopeExit(TFunc&& func)
				:m_Func(std::forward<TFunc>(func))
			{
			}
			~CallAtScopeExit()
			{
				std::invoke(m_Func);
			}
	};
}
