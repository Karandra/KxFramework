#pragma once
#include "Event.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::EventSystem
{
	template<class TMethod_>
	class ParametrizedInvocationEvent: public RTTI::ImplementInterface<ParametrizedInvocationEvent<TMethod_>, BasicEvent, IParametrizedInvocationEvent>
	{
		protected:
			using TArgsTuple = typename Utility::MethodTraits<TMethod_>::TArgsTuple;
			using TResult = typename Utility::MethodTraits<TMethod_>::TReturn;

		protected:
			TArgsTuple m_Parameters;
			TResult m_Result;

		public:
			template<class... Args>
			ParametrizedInvocationEvent(Args&&... arg)
				:m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<ParametrizedInvocationEvent>(std::move(*this));
			}

			// IParametrizedInvocationEvent
			void GetParameters(void* parameters) override
			{
				*static_cast<TArgsTuple*>(parameters) = std::move(m_Parameters);
			}

			void GetResult(void* value) override
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
