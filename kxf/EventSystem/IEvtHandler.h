#pragma once
#include "Common.h"
#include "Event.h"
#include "EventID.h"
#include "EventItem.h"
#include "EventBuilder.h"
#include "EventExecutor.h"
#include "kxf/RTTI/QueryInterface.h"
#include "kxf/wxWidgets/IWithEvent.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf
{
	class IEventExecutor;
	class EvtHandlerDelegate;
}
namespace kxf::EventSystem
{
	class EvtHandlerAccessor;
}

namespace kxf::EventSystem::Private
{
	template<class TEvent, class TFunc>
	void CallWxEvent(IEvent& event, TFunc&& func) noexcept(std::is_nothrow_invocable_v<TFunc, TEvent&>)
	{
		if (auto withEvent = event.QueryInterface<wxWidgets::IWithEvent>())
		{
			std::invoke(func, static_cast<TEvent&>(withEvent->GetEvent()));
		}
		else
		{
			event.Skip();
		}
	}
}

namespace kxf
{
	class KX_API IEvtHandler: public RTTI::Interface<IEvtHandler>
	{
		friend class EventSystem::EvtHandlerAccessor;

		KxDeclareIID(IEvtHandler, {0x96ae0970, 0x8cc5, 0x4288, {0xb1, 0x1b, 0x7a, 0xe6, 0x42, 0xf8, 0xdf, 0x8c}});

		protected:
			using EventItem = EventSystem::EventItem;

		private:
			template<class TCallable, class... Args>
			void DoCallAfter(UniversallyUniqueID id, TCallable&& callable, Args&&... arg)
			{
				using namespace EventSystem;
				using Traits = typename Utility::CallableTraits<TCallable, Args...>;

				if constexpr(Traits::IsMemberFunction)
				{
					DoQueueEvent(std::make_unique<MethodIndirectInvocation<TCallable, Args...>>(*this, callable, std::forward<Args>(arg)...), IIndirectInvocationEvent::EvtIndirectInvocation, std::move(id));
				}
				else if constexpr(Traits::IsInvokable)
				{
					DoQueueEvent(std::make_unique<CallableIndirectInvocation<TCallable, Args...>>(*this, std::forward<TCallable>(callable), std::forward<Args>(arg)...), IIndirectInvocationEvent::EvtIndirectInvocation, std::move(id));
				}
				else
				{
					static_assert(false, "Unsupported callable type or the type is not invocable");
				}
			}

			template<class TCallable, class... Args>
			bool DoCallHere(TCallable&& callable, Args&&... arg)
			{
				using namespace EventSystem;
				using Traits = typename Utility::CallableTraits<TCallable, Args...>;

				if constexpr(Traits::IsMemberFunction)
				{
					MethodIndirectInvocation<TCallable, Args...> event(*this, callable, std::forward<Args>(arg)...);
					return DoProcessEvent(event, IIndirectInvocationEvent::EvtIndirectInvocation);
				}
				else if constexpr(Traits::IsInvokable)
				{
					CallableIndirectInvocation<TCallable, Args...> event(*this, std::forward<TCallable>(callable), std::forward<Args>(arg)...);
					return DoProcessEvent(event, IIndirectInvocationEvent::EvtIndirectInvocation);
				}
				else
				{
					static_assert(false, "Unsupported callable type or the type is not invocable");
					return false;
				}
			}

		protected:
			virtual LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {}) = 0;
			virtual bool DoUnbind(const EventID& eventID, IEventExecutor& executor) = 0;
			virtual bool DoUnbind(const LocallyUniqueID& bindSlot) = 0;

			virtual bool OnDynamicBind(EventItem& eventItem) = 0;
			virtual bool OnDynamicUnbind(EventItem& eventItem) = 0;

			virtual void DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}) = 0;
			virtual bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, IEvtHandler* onlyIn = nullptr) = 0;
			virtual bool DoProcessEventSafely(IEvent& event, const EventID& eventID = {}) = 0;
			virtual bool DoProcessEventLocally(IEvent& event, const EventID& eventID = {}) = 0;

			virtual bool TryBefore(IEvent& event) = 0;
			virtual bool TryAfter(IEvent& event) = 0;

		public:
			IEvtHandler() = default;
			virtual ~IEvtHandler() = default;

		public:
			// Event queuing and processing
			virtual bool ProcessPendingEvents() = 0;
			virtual size_t DiscardPendingEvents() = 0;

			// Event handlers chain
			virtual IEvtHandler* GetPrevHandler() const = 0;
			virtual IEvtHandler* GetNextHandler() const = 0;
			virtual void SetPrevHandler(IEvtHandler* evtHandler) = 0;
			virtual void SetNextHandler(IEvtHandler* evtHandler) = 0;

			virtual void Unlink() = 0;
			virtual bool IsUnlinked() const = 0;

			virtual bool IsEventProcessingEnabled() const = 0;
			virtual void EnableEventProcessing(bool enable = true) = 0;

		public:
			// Bind free or static function
			template<class TEvent, class TEventArg>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, void(*func)(TEventArg&), FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return DoBind(eventTag, std::make_unique<EventSystem::CallableEventExecutor<TEvent, decltype(func)>>(func), flags);
			}

			// Unbind free or static function
			template<class TEvent, class TEventArg>
			bool Unbind(const EventTag<TEvent>& eventTag, void(*func)(TEventArg&))
			{
				EventSystem::CallableEventExecutor<TEvent, decltype(func)> executor(func);
				return DoUnbind(eventTag, executor);
			}

			// Bind a generic callable
			template<class TEvent, class TCallable>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, TCallable&& callable, FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return DoBind(eventTag, std::make_unique<EventSystem::CallableEventExecutor<TEvent, TCallable>>(std::forward<TCallable>(callable)), flags);
			}

			// Unbind a generic callable
			template<class TEvent, class TCallable>
			bool Unbind(const EventTag<TEvent>& eventTag, const TCallable& callable)
			{
				EventSystem::CallableEventExecutor<TEvent, TCallable> executor(std::forward<TCallable>(callable));
				return DoUnbind(eventTag, executor);
			}

			// Bind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler>
			LocallyUniqueID Bind(const EventTag<TEvent>& eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler, FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return DoBind(eventTag, std::make_unique<EventSystem::MethodEventExecutor<TEvent, TClass, TEventArg, TEventHandler>>(method, *handler), flags);
			}

			// Unbind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler>
			bool Unbind(const EventTag<TEvent>& eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler)
			{
				EventSystem::MethodEventExecutor<TEvent, TClass, TEventArg, TEventHandler> executor(method, *handler);
				return DoUnbind(eventTag, executor);
			}

			// Unbind by a given bind slot
			bool Unbind(LocallyUniqueID bindSlot)
			{
				return DoUnbind(bindSlot);
			}

			// Unbind all events
			bool UnbindAll()
			{
				return DoUnbind(IEvent::EvtAny, EventSystem::NullEventExecutor::Get());
			}

			// Unbind all events with this ID
			bool UnbindAll(const EventID& id)
			{
				return id != IEvent::EvtAny && DoUnbind(id, EventSystem::NullEventExecutor::Get());
			}

		public:
			// Bind a generic callable
			template<class TMethod, class TCallable>
			LocallyUniqueID BindParametrized(TMethod method, TCallable&& callable, FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return DoBind(EventID(method), std::make_unique<EventSystem::ParametrizedCallableEventExecutor<TMethod, TCallable>>(std::forward<TCallable>(callable)), flags);
			}

			template<class TMethod, class... Args>
			std::enable_if_t<std::is_invocable_v<TMethod, typename Utility::MethodTraits<TMethod>::TInstance, Args...>, typename Utility::MethodTraits<TMethod>::TReturn>
				ProcessParametrizedEvent(TMethod method, Args&&... arg)
			{
				EventSystem::ParametrizedInvocationEvent<TMethod> event(std::forward<Args>(arg)...);
				if (DoProcessEvent(event, EventID(method)))
				{
					using TResult = typename Utility::MethodTraits<TMethod>::TReturn;
					if constexpr(!std::is_void_v<TResult>)
					{
						TResult result;
						event.GetResult(&result);
						return result;
					}
				}
				return {};
			}

		public:
			// [WX] Bind free or static function
			template<class TEvent, class TEventArg, class = std::enable_if_t<std::is_base_of_v<wxEvent, TEvent>>>
			LocallyUniqueID BindWx(wxEventTypeTag<TEvent> eventTag, void(*func)(TEventArg&), FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return Bind(EventTag<IEvent>(eventTag), [func](IEvent& event)
				{
					EventSystem::Private::CallWxEvent<TEventArg>(event, func);
				}, flags);
			}

			// [WX] Bind a generic callable
			template<class TEvent, class TCallable, class = std::enable_if_t<std::is_base_of_v<wxEvent, TEvent>>>
			LocallyUniqueID BindWx(wxEventTypeTag<TEvent> eventTag, TCallable&& callable, FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return Bind(EventTag<IEvent>(eventTag), [callable = std::forward<TCallable>(callable)](IEvent& event)
				{
					EventSystem::Private::CallWxEvent<TEvent>(event, callable);
				}, flags);
			}

			// [WX] Bind a member function
			template<class TEvent, class TClass, class TEventArg, class TEventHandler, class = std::enable_if_t<std::is_base_of_v<wxEvent, TEvent>>>
			LocallyUniqueID BindWx(wxEventTypeTag<TEvent> eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler, FlagSet<EventFlag> flags = EventFlag::Direct)
			{
				return Bind(EventTag<IEvent>(eventTag), [method, handler](IEvent& event)
				{
					EventSystem::Private::CallWxEvent<TEventArg>(event, [&](TEventArg& event)
					{
						std::invoke(method, handler, event);
					});
				}, flags);
			}

		public:
			bool ProcessEvent(IEvent& event, const EventID& eventID = {})
			{
				return DoProcessEvent(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEvent(const EventID& eventID, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEvent(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEvent(event, eventTag);
			}

			// Processes an event by calling 'DoProcessEvent' and handles any exceptions that occur in the process
			bool ProcessEventSafely(IEvent& event, const EventID& eventID)
			{
				return DoProcessEventSafely(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventSafely(const EventID& eventID, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEventSafely(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventSafely(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEventSafely(event, eventTag);
			}

			// Try to process the event in this handler and all those chained to it
			bool ProcessEventLocally(IEvent& event, const EventID& eventID)
			{
				return DoProcessEventLocally(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventLocally(const EventID& eventID, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEventLocally(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventLocally(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return DoProcessEventLocally(event, eventTag);
			}

			// Construct and send the event using the event builder
			template<class TEvent, class... Args>
			decltype(auto) BuildProcessEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return EventSystem::DirectEventBuilder<TEvent>(*this, TEvent(std::forward<Args>(arg)...), eventTag);
			}

		public:
			// Queue event for a later processing
			void QueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {})
			{
				DoQueueEvent(std::move(event), eventID, std::move(uuid));
			}

			template<class TEvent, class... Args>
			void QueueEvent(const EventID& eventID, Args&&... arg)
			{
				DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventID);
			}

			template<class TEvent, class... Args>
			void QueueEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag);
			}

			template<class TEvent, class... Args>
			void QueueUniqueEvent(UniversallyUniqueID uuid, const EventID& eventID, Args&&... arg)
			{
				DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventID, std::move(uuid));
			}

			template<class TEvent, class... Args>
			void QueueUniqueEvent(UniversallyUniqueID uuid, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag, std::move(uuid));
			}

			// Construct and queue event using the event builder
			template<class TEvent, class... Args>
			decltype(auto) BuildQueueEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return EventSystem::QueuedEventBuilder<TEvent>(*this, std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag);
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

			// Executes given callable as an anonymous event
			template<class TCallable, class... Args>
			bool CallHere(TCallable&& callable, Args&&... arg)
			{
				return DoCallHere(std::forward<TCallable>(callable), std::forward<Args>(arg)...);
			}
	};
}
