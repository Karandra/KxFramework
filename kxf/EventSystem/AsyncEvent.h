#pragma once
#include "Event.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::EventSystem
{
	class AsyncEvent: public RTTI::ImplementInterface<AsyncEvent, BasicEvent, IAsyncEvent>
	{
		public:
			KxEVENT_MEMBER_AS(AsyncEvent, Async, -2);

		private:
			EvtHandler* m_EvtHandler = nullptr;

		public:
			AsyncEvent(EvtHandler& evtHandler)
				:m_EvtHandler(&evtHandler)
			{
				BasicEvent::SetEventSource(&evtHandler);
			}

		public:
			// IEvent
			EventID GetEventID() const override
			{
				return EvtAsync;
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return EventCategory::Unknown;
			}

			EvtHandler* GetEventSource() const override
			{
				return m_EvtHandler;
			}
			void SetEventSource(EvtHandler* evtHandler) override
			{
			}

			bool IsSkipped() const override
			{
				return false;
			}
			void Skip(bool skip = true) override
			{
			}

			bool IsAllowed() const override
			{
				return true;
			}
			void Allow(bool allow = true) override
			{
			}
	};
}

namespace kxf::EventSystem
{
	// Wrapper for free/static/lambda function or a class that implements 'operator()'
	template<class TCallable, class... Args>
	class CallableAsyncEvent: public AsyncEvent
	{
		protected:
			TCallable m_Callable;
			std::tuple<Args...> m_Parameters;

		public:
			CallableAsyncEvent(EvtHandler& evtHandler, TCallable&& callable, Args&&... arg)
				:AsyncEvent(evtHandler), m_Callable(std::forward<TCallable>(callable)), m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<CallableAsyncEvent>(std::move(*this));
			}

			// IAsyncEvent
			void Execute() override
			{
				std::apply(m_Callable, std::move(m_Parameters));
			}
	};

	// Wrapper for class member function
	template<class TMethod, class... Args>
	class MethodAsyncEvent: public AsyncEvent
	{
		protected:
			using TClass = typename Utility::MethodTraits<TMethod>::TInstance;

		protected:
			std::tuple<Args...> m_Parameters;
			TMethod m_Method = nullptr;
			TClass* m_EvtHandler = nullptr;

		public:
			MethodAsyncEvent(EvtHandler& evtHandler, TMethod method, Args&&... arg)
				:AsyncEvent(evtHandler), m_EvtHandler(static_cast<TClass*>(&evtHandler)), m_Method(method), m_Parameters(std::forward<Args>(arg)...)
			{
				static_assert(std::is_base_of_v<EvtHandler, TClass>, "EvtHandler descendant required");
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<MethodAsyncEvent>(std::move(*this));
			}

			// IAsyncEvent
			void Execute() override
			{
				std::apply([this](auto&&... arg)
				{
					std::invoke(m_Method, m_EvtHandler, std::forward<decltype(arg)>(arg)...);
				}, std::move(m_Parameters));
			}
	};
}
