#pragma once
#include "IEventExecutor.h"
#include "IEvent.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include "kxf/Utility/TypeTraits.h"
#include "kxf/Utility/Memory.h"

namespace kxf::EventSystem::Private
{
	template<class TArgsTuple, class TResult, class TFunc>
	void ExecuteWithParameters(IEvent& event, TFunc&& func)
	{
		object_ptr<ISignalInvocationEvent> parameterizedInvocation;
		if (event.QueryInterface(parameterizedInvocation))
		{
			alignas(TArgsTuple) uint8_t parametersBuffer[sizeof(TArgsTuple)] = {};
			if (parameterizedInvocation->GetParameters(parametersBuffer))
			{
				TArgsTuple& parameters = *std::launder(reinterpret_cast<TArgsTuple*>(parametersBuffer));
				Utility::CallAtScopeExit atExit = [&]()
				{
					Utility::DestroyObjectOnMemoryLocation<TArgsTuple>(parametersBuffer);
				};

				if constexpr(!std::is_void_v<TResult>)
				{
					TResult result = std::invoke(func, std::move(parameters));
					if (!event.IsSkipped())
					{
						parameterizedInvocation->PutResult(&result);
					}
				}
				else
				{
					std::invoke(func, std::move(parameters));
				}
			}
		}
	}
}

namespace kxf::EventSystem
{
	template<class TSignal_, class TCallable_, BindSignalFlag signalFlags>
	class CallableSignalExecutor: public IEventExecutor
	{
		protected:
			using TCallable = TCallable_;

			using TArgsTuple = typename Utility::MethodTraits<TSignal_>::TArgsTuple;
			using TResult = typename Utility::MethodTraits<TSignal_>::TReturn;

		protected:
			TCallable m_Callable;
			const void* m_OriginalAddress = nullptr;

		public:
			CallableSignalExecutor(TCallable&& func)
				:m_Callable(std::forward<TCallable>(func)), m_OriginalAddress(std::addressof(func))
			{
			}

		public:
			void Execute(IEvtHandler& evtHandler, IEvent& event) override
			{
				Private::ExecuteWithParameters<TArgsTuple, TResult>(event, [&](TArgsTuple&& parameters)
				{
					if constexpr(FlagSet<BindSignalFlag>(signalFlags).Contains(BindSignalFlag::EventContext))
					{
						return std::apply([&](auto&&... arg)
						{
							return std::invoke(m_Callable, event, std::forward<decltype(arg)>(arg)...);
						}, std::move(parameters));
					}
					else
					{
						return std::apply(m_Callable, std::move(parameters));
					}
				});
			}
			bool IsSameAs(const IEventExecutor& other) const noexcept override
			{
				if (typeid(*this) == typeid(other))
				{
					return m_OriginalAddress == static_cast<const CallableSignalExecutor&>(other).m_OriginalAddress;
				}
				return false;
			}
			IEvtHandler* GetTargetHandler() noexcept override
			{
				return nullptr;
			}
	};

	template<class TSignal_, class THandlerMethod_, class THandlerObject_, BindSignalFlag signalFlags>
	class MethodSignalExecutor: public IEventExecutor
	{
		public:
			using THandlerObject = THandlerObject_;
			using THandlerMethod = THandlerMethod_;

			using TArgsTuple = typename Utility::MethodTraits<TSignal_>::TArgsTuple;
			using TResult = typename Utility::MethodTraits<TSignal_>::TReturn;

		protected:
			THandlerObject* m_HandlerObject = nullptr;
			THandlerMethod m_HandlerMethod = nullptr;

		public:
			MethodSignalExecutor(THandlerMethod handlerMethod, THandlerObject& handlerObject)
				:m_HandlerMethod(handlerMethod), m_HandlerObject(&handlerObject)
			{
			}

		public:
			void Execute(IEvtHandler& evtHandler, IEvent& event) override
			{
				THandlerObject* handlerObject = m_HandlerObject;
				if (!handlerObject)
				{
					if constexpr(std::is_base_of_v<IEvtHandler, THandlerObject>)
					{
						handlerObject = static_cast<THandlerObject*>(&evtHandler);
					}
				}

				Private::ExecuteWithParameters<TArgsTuple, TResult>(event, [&](TArgsTuple&& parameters)
				{
					return std::apply([&](auto&&... arg)
					{
						if constexpr(FlagSet<BindSignalFlag>(signalFlags).Contains(BindSignalFlag::EventContext))
						{
							return std::invoke(m_HandlerMethod, handlerObject, event, std::forward<decltype(arg)>(arg)...);
						}
						else
						{
							return std::invoke(m_HandlerMethod, handlerObject, std::forward<decltype(arg)>(arg)...);
						}
					}, std::move(parameters));
				});
			}
			bool IsSameAs(const IEventExecutor& other) const noexcept override
			{
				if (typeid(*this) == typeid(other))
				{
					const MethodSignalExecutor& otherRef = static_cast<const MethodSignalExecutor&>(other);

					return (m_HandlerMethod == otherRef.m_HandlerMethod || otherRef.m_HandlerMethod == nullptr) &&
						(m_HandlerObject == otherRef.m_HandlerObject || otherRef.m_HandlerObject == nullptr);
				}
				return false;
			}
			IEvtHandler* GetTargetHandler() noexcept override
			{
				if constexpr(std::is_base_of_v<IEvtHandler, THandlerObject>)
				{
					return m_HandlerObject;
				}
				return nullptr;
			}
	};
}
