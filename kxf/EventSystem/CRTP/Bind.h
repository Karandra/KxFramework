#pragma once
#include "Common.h"
#include "../Event.h"
#include "../EventExecutor.h"

namespace kxf::EventSystem
{
	template<class T>
	class BindCRTP
	{
		private:
			T& Self()
			{
				return static_cast<T&>(*this);
			}

		public:
			// Bind free or static function
			template<class TEvent, class TEventArg>
			LocallyUniqueID Bind(const EventTag<TEvent>&& eventTag, void(*func)(TEventArg&), FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return Self().DoBind(eventTag, std::make_unique<EventSystem::CallableEventExecutor<TEvent, decltype(func)>>(func), flags);
			}

			// Unbind free or static function
			template<class TEvent, class TEventArg>
			bool Unbind(const EventTag<TEvent>& eventTag, void(*func)(TEventArg&))
			{
				EventSystem::CallableEventExecutor<TEvent, decltype(func)> executor(func);
				return Self().DoUnbind(eventTag, executor);
			}

			// Bind a generic callable (lambda function)
			template<class TEvent, class TCallable>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, TCallable&& callable, FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return Self().DoBind(eventTag, std::make_unique<EventSystem::CallableEventExecutor<TEvent, TCallable>>(std::forward<TCallable>(callable)), flags);
			}

			// Unbind a generic callable (lambda function)
			template<class TEvent, class TCallable>
			bool Unbind(const EventTag<TEvent>& eventTag, const TCallable& callable)
			{
				EventSystem::CallableEventExecutor<TEvent, TCallable> executor(std::forward<TCallable>(callable));
				return Self().DoUnbind(eventTag, executor);
			}

			// Bind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler, FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return Self().DoBind(eventTag, std::make_unique<EventSystem::MethodEventExecutor<TEvent, TClass, TEventArg, TEventHandler>>(method, *handler), flags);
			}

			// Unbind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler>
			bool Unbind(const EventTag<TEvent>& eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler)
			{
				EventSystem::MethodEventExecutor<TEvent, TClass, TEventArg, TEventHandler> executor(method, *handler);
				return Self().DoUnbind(eventTag, executor);
			}

			// Unbind by a given bind slot
			bool Unbind(LocallyUniqueID bindSlot)
			{
				return Self().DoUnbind(bindSlot);
			}

			// Unbind all events
			bool UnbindAll()
			{
				return Self().DoUnbind(Event::EvtAny, NullEventExecutor::Get());
			}

			// Unbind all events with this ID
			bool UnbindAll(const EventID& id)
			{
				return id != Event::EvtAny && Self().DoUnbind(id, NullEventExecutor::Get());
			}
	};
}
