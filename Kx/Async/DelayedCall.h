#pragma once
#include "Common.h"
#include "Coroutine.h"

namespace KxFramework::Async
{
	template<class TCallable>
	static void DelayedCall(TCallable&& func, TimeSpan delay)
	{
		enum class State
		{
			Wait,
			Execute
		};

		Coroutine::Run([delay, func = std::forward<TCallable>(func)](Coroutine& coroutine)
		{
			if (coroutine.GetNextState<State::Execute>())
			{
				std::invoke(func);
				return Coroutine::YieldStop();
			}
			else
			{
				return Coroutine::YieldWait(delay, State::Execute);
			}
		});
	}
}
