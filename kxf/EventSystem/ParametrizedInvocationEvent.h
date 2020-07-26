#pragma once
#include "Event.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::EventSystem
{
	class ParametrizedInvocationEvent: public RTTI::ImplementInterface<ParametrizedInvocationEvent, BasicEvent, IParametrizedInvocationEvent>
	{
	};
}

namespace kxf::EventSystem
{
	// Wrapper for free/static/lambda function or a class that implements 'operator()'
	template<class TArgsTuple>
	class CallableParametrizedInvocation: public ParametrizedInvocationEvent
	{
		protected:
			TArgsTuple m_Parameters;

		public:
			template<class... Args>
			CallableParametrizedInvocation(Args&&... arg)
				:m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<CallableParametrizedInvocation>(std::move(*this));
			}

			// IParametrizedInvocationEvent
			void GetParameters(void* parameters) override
			{
				*static_cast<TArgsTuple*>(parameters) = std::move(m_Parameters);
			}
	};
}
