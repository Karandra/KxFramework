#pragma once
#include "Event.h"
#include "kxf/Utility/TypeTraits.h"
#include "kxf/Utility/Memory.h"

namespace kxf::EventSystem
{
	template<SignalParametersSemantics signalSemantics, class TMethod_>
	class SignalInvocationEvent: public RTTI::ImplementInterface<SignalInvocationEvent<signalSemantics, TMethod_>, BasicEvent, ISignalInvocationEvent>
	{
		protected:
			using TArgsTuple = typename Utility::MethodTraits<TMethod_>::TArgsTuple;
			using TResult = typename Utility::MethodTraits<TMethod_>::TReturn;

		protected:
			TArgsTuple m_Parameters;
			std::conditional_t<!std::is_void_v<TResult>, TResult, void*> m_Result;

		public:
			template<class... Args>
			SignalInvocationEvent(Args&&... arg)
				:m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<SignalInvocationEvent>(std::move(*this));
			}

			// ISignalInvocationEvent
			bool GetParameters(void* parameters) override
			{
				// Nothing to deallocate, so the deallocation function is empty
				if constexpr(signalSemantics == SignalParametersSemantics::Copy)
				{
					return Utility::NewObjectOnMemoryLocation<TArgsTuple>(parameters, []()
					{
					}, m_Parameters) != nullptr;
				}
				else if constexpr(signalSemantics == SignalParametersSemantics::Move)
				{
					return Utility::NewObjectOnMemoryLocation<TArgsTuple>(parameters, []()
					{
					}, std::move(m_Parameters)) != nullptr;
				}
				else
				{
					static_assert(false, "incorrect 'SignalParametersSemantics' option");
				}
			}

			void TakeResult(void* value) override
			{
				if constexpr(!std::is_void_v<TResult>)
				{
					*static_cast<TResult*>(value) = std::move(m_Result);
				}
			}
			void PutResult(void* value) override
			{
				if constexpr(!std::is_void_v<TResult>)
				{
					m_Result = std::move(*static_cast<TResult*>(value));
				}
			}
	};
}
