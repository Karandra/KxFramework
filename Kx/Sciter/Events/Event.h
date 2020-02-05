#pragma once
#include "Kx/Sciter/Common.h"
#include "Kx/Sciter/Element.h"
#include "Kx/EventSystem/Event.h"
#include "Kx/EventSystem/Events/NotifyEvent.h"

namespace KxSciter
{
	class KX_API Event: public KxNotifyEvent
	{
		public:
			KxEVENT_MEMBER(Event, Attached);
			KxEVENT_MEMBER(Event, Detached);

			KxEVENT_MEMBER(Event, EngineCreated);
			KxEVENT_MEMBER(Event, EngineDestroyed);

			KxEVENT_MEMBER(Event, Gesture);
			KxEVENT_MEMBER(Event, Exchange);

			KxEVENT_MEMBER(Event, ContentLoaded);
			KxEVENT_MEMBER(Event, TIScriptCall);
			KxEVENT_MEMBER(Event, ScriptingCall);

		protected:
			Host* m_Host = nullptr;
			ElementUID* m_Element = nullptr;
			ElementUID* m_TargetElement = nullptr;

		protected:
			Element RetrieveElement(ElementUID* uid) const;
			void AcceptElement(ElementUID*& uid, const Element& element);

		public:
			Event(Host& host)
				:m_Host(&host)
			{
			}

		public:
			Event* Clone() const override
			{
				return new Event(*this);
			}
			Host& GetHost() const
			{
				return *m_Host;
			}

			Element GetElement() const
			{
				return RetrieveElement(m_Element);
			}
			void SetElement(const Element& element)
			{
				AcceptElement(m_Element, element);
			}
	
			Element GetTargetElement() const
			{
				return RetrieveElement(m_TargetElement);
			}
			void SetTargetElement(const Element& element)
			{
				AcceptElement(m_TargetElement, element);
			}
	};
}

namespace KxSciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, Attached);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, Detached);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, EngineCreated);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, EngineDestroyed);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, Gesture);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, Exchange);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, ContentLoaded);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, TIScriptCall);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(Event, ScriptingCall);
}
