#pragma once
#include "Kx/EventSystem/Event.h"
#include "Kx/Utility/TypeTraits.h"
#include <functional>
#include <type_traits>

namespace kxf
{
	class IIndirectCallEvent: public wxAsyncMethodCallEvent
	{
		public:
			// Event for anonymous execution of a given callable (see EvtHandler::CallAfter)
			KxEVENT_MEMBER_AS(IIndirectCallEvent, IndirectCall, wxEVT_ASYNC_METHOD_CALL);

		protected:
			wxEvtHandler& GetEvtHandler()
			{
				return *static_cast<wxEvtHandler*>(GetEventObject());
			}

		public:
			IIndirectCallEvent(wxEvtHandler& evtHandler)
				:wxAsyncMethodCallEvent(&evtHandler)
			{
			}

		public:
			IIndirectCallEvent* Clone() const override
			{
				return nullptr;
			}
	};
}

namespace kxf::EventSystem
{
	// Wrapper for lambda function or a class which implements 'operator()'
	template<class TCallable, class... Args>
	class FunctorIndirectCall: public IIndirectCallEvent
	{
		protected:
			TCallable m_Callable;
			std::tuple<Args...> m_Parameters;

		public:
			FunctorIndirectCall(wxEvtHandler& evtHandler, TCallable callable, Args&&... arg)
				:IIndirectCallEvent(evtHandler), m_Callable(std::move(callable)), m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			void Execute() override
			{
				std::apply(m_Callable, std::move(m_Parameters));
			}
	};

	// Wrapper for free or static function
	template<class TFunction, class... Args>
	class FunctionIndirectCall: public IIndirectCallEvent
	{
		protected:
			std::tuple<Args...> m_Parameters;
			TFunction m_Function;

		public:
			FunctionIndirectCall(wxEvtHandler& evtHandler, TFunction func, Args&&... arg)
				:IIndirectCallEvent(evtHandler), m_Function(func), m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			void Execute() override
			{
				std::apply(m_Function, std::move(m_Parameters));
			}
	};

	// Wrapper for class member function
	template<class TMethod, class... Args>
	class MethodIndirectCall: public IIndirectCallEvent
	{
		protected:
			std::tuple<Args...> m_Parameters;
			TMethod m_Method = nullptr;

		public:
			MethodIndirectCall(wxEvtHandler& evtHandler, TMethod func, Args&&... arg)
				:IIndirectCallEvent(evtHandler), m_Method(func), m_Parameters(std::forward<Args>(arg)...)
			{
			}

		public:
			void Execute() override
			{
				std::apply([this](auto&& ... arg)
				{
					using TClass = typename Utility::MethodTraits<TMethod>::TInstance;
					std::invoke(m_Method, static_cast<TClass*>(&GetEvtHandler()), std::forward<decltype(arg)>(arg)...);
				}, std::move(m_Parameters));
			}
	};
}
