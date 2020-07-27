#pragma once
#include "Event.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::EventSystem
{
	template<EventParametersSemantics parameterSemantics, class TMethod_>
	class ParameterizedInvocationEvent: public RTTI::ImplementInterface<ParameterizedInvocationEvent<parameterSemantics, TMethod_>, BasicEvent, IParameterizedInvocationEvent>
	{
		protected:
			using TArgsTuple = typename Utility::MethodTraits<TMethod_>::TArgsTuple;
			using TResult = typename Utility::MethodTraits<TMethod_>::TReturn;

		protected:
			TArgsTuple m_Parameters;
			TResult m_Result;

		public:
			template<class... Args>
			ParameterizedInvocationEvent(Args&&... arg)
				:m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ParameterizedInvocationEvent>(std::move(*this));
			}

			// IParameterizedInvocationEvent
			void GetParameters(void* parameters) override
			{
				if constexpr(parameterSemantics == EventParametersSemantics::Copy)
				{
					*static_cast<TArgsTuple*>(parameters) = m_Parameters;
				}
				else if constexpr(parameterSemantics == EventParametersSemantics::Move)
				{
					*static_cast<TArgsTuple*>(parameters) = std::move(m_Parameters);
				}
				else
				{
					static_assert(false, "incorrect 'EventParametersSemantics' option");
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
