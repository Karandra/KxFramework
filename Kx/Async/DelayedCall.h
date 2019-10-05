#pragma once
#include "Common.h"
#include "Coroutine.h"

namespace KxAsync
{
	template<class TCallable> static void DelayedCall(TCallable func, const wxTimeSpan& delay)
	{
		KxCoroutine::Run([delay, func = std::move(func)](KxCoroutine& coroutine)
		{
			if (coroutine.GetNextState())
			{
				func();
				return KxCoroutine::YieldStop();
			}
			else
			{
				return KxCoroutine::YieldWait(delay, true);
			}
		});
	}
}
