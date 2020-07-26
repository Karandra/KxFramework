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

		protected:
			TArgsTuple m_Parameters;

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
	};
}
