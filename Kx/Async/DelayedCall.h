#pragma once
#include "Common.h"
#include "Coroutine.h"

namespace KxFramework::Async
{
	template<class TCallable>
	static void DelayedCall(TCallable&& func, const wxTimeSpan& delay)
	{
		Coroutine::Run([delay, func = std::forward<TCallable>(func)](Coroutine& coroutine)
		{
			if (coroutine.GetNextState())
			{
				func();
				return Coroutine::YieldStop();
			}
			else
			{
				return Coroutine::YieldWait(delay, true);
			}
		});
	}
}
