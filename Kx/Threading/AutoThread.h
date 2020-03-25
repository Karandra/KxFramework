#pragma once
#include "Common.h"
#include "Utility/TypeTraits.h"
#include <wx/thread.h>

namespace KxFramework
{
	template<class TCallable>
	class KX_API AutoThread: public wxThread
	{
		private:
			TCallable m_Callable;

		protected:
			ExitCode Entry() override
			{
				using TReturn = typename Utility::MethodTraits<TCallable>::TReturn;

				if constexpr(std::is_void_v<TReturn>)
				{
					std::invoke(m_Callable);
				}
				else if constexpr(std::is_same_v<TReturn, bool>)
				{
					return static_cast<ExitCode>(std::invoke(m_Callable) ? 0 : std::numeric_limits<ExitCode>::max());
				}
				else if constexpr(std::is_integral_v<TReturn> || std::is_enum_v<TReturn> || std::is_pointer_v<TReturn>)
				{
					return static_cast<ExitCode>(std::invoke(m_Callable));
				}
				else
				{
					static_assert(false, "not supported return type (use either integral type or void)");
				}
				return nullptr;
			}

		public:
			AutoThread(TCallable&& callable)
				:wxThread(wxThreadKind::wxTHREAD_JOINABLE), m_Callable(std::forward<TCallable>(callable))
			{
			}
	};
}
