#pragma once
#include "Event.h"
#include "kxf/General/AlignedStorage.h"
#include "kxf/Utility/TypeTraits.h"
#include "kxf/Utility/Memory.h"

namespace kxf::EventSystem
{
	template<SignalParametersSemantics signalSemantics, class TSignal_>
	class SignalInvocationEvent: public RTTI::Implementation<SignalInvocationEvent<signalSemantics, TSignal_>, BasicEvent, ISignalInvocationEvent>
	{
		protected:
			using TArgsTuple = typename Utility::MethodTraits<TSignal_>::TArgsTuple;
			using TResult = typename Utility::MethodTraits<TSignal_>::TReturn;

			using TActualResult = std::conditional_t<!std::is_void_v<TResult>, TResult, void*>;

		protected:
			TArgsTuple m_Parameters;
			std::optional<TActualResult> m_Result;

		public:
			template<class... Args, std::enable_if_t<std::is_constructible_v<TArgsTuple, Args...>, int> = 0>
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
				if constexpr(signalSemantics == SignalParametersSemantics::Copy)
				{
					return Utility::ConstructAt<TArgsTuple>(parameters, m_Parameters) != nullptr;
				}
				else if constexpr(signalSemantics == SignalParametersSemantics::Move)
				{
					return Utility::ConstructAt<TArgsTuple>(parameters, std::move(m_Parameters)) != nullptr;
				}
				else
				{
					static_assert(sizeof(TSignal_*) == 0, "incorrect 'SignalParametersSemantics' option");
				}
			}

			void TakeResult(void* value) override
			{
				if constexpr(!std::is_void_v<TResult>)
				{
					*static_cast<TResult*>(value) = std::move(*m_Result);
					m_Result = std::nullopt;
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
