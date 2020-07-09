#pragma once
#include "../Common.h"
#include "../AsyncEvent.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::EventSystem
{
	template<class T>
	class CallAfterCRTP
	{
		private:
			T& Self()
			{
				return static_cast<T&(*this);
			}

		private:
			template<class TCallable, class... Args>
			void DoCallAfter(UniversallyUniqueID id, TCallable&& callable, Args&&... arg)
			{
				using Traits = typename Utility::CallableTraits<TCallable, Args...>;

				std::unique_ptr<Event> event;
				if constexpr(Traits::IsMemberFunction)
				{
					event = std::make_unique<MethodAsyncEvent<TCallable, Args...>>(Self(), callable, std::forward<Args>(arg)...);
				}
				else if constexpr(Traits::IsInvokable)
				{
					event = std::make_unique<CallableAsyncEvent<TCallable, Args...>>(Self(), std::forward<TCallable>(callable), std::forward<Args>(arg)...);
				}
				else
				{
					static_assert(false, "Unsupported callable type or the type is not invokable");
				}

				if (event)
				{
					Self().DoQueueEvent(std::move(event), AsyncEvent::EvtAsync, std::move(id));
				}
			}

		public:
			// Queue execution of a given callable to the next event loop iteration
			template<class TCallable, class... Args>
			void CallAfter(TCallable&& callable, Args&&... arg)
			{
				DoCallAfter({}, std::forward<TCallable>(callable), std::forward<Args>(arg)...);
			}

			// Queue execution of a given callable to the next event loop iteration replacing previously sent callable with the same ID
			template<class TCallable, class... Args>
			void UniqueCallAfter(UniversallyUniqueID id, TCallable&& callable, Args&&... arg)
			{
				DoCallAfter(std::move(id), std::forward<TCallable>(callable), std::forward<Args>(arg)...);
			}
	};
}
