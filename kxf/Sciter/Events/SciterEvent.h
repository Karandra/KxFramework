#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Sciter/Element.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class KX_API SciterEvent: public BasicEvent
	{
		public:
			KxEVENT_MEMBER(SciterEvent, Attached);
			KxEVENT_MEMBER(SciterEvent, Detached);

			KxEVENT_MEMBER(SciterEvent, EngineCreated);
			KxEVENT_MEMBER(SciterEvent, EngineDestroyed);

			KxEVENT_MEMBER(SciterEvent, Gesture);
			KxEVENT_MEMBER(SciterEvent, Exchange);

			KxEVENT_MEMBER(SciterEvent, ContentLoaded);
			KxEVENT_MEMBER(SciterEvent, TIScriptCall);
			KxEVENT_MEMBER(SciterEvent, ScriptingCall);

		protected:
			Sciter::Host* m_Host = nullptr;
			Sciter::ElementUID* m_Element = nullptr;
			Sciter::ElementUID* m_TargetElement = nullptr;

		protected:
			Sciter::Element RetrieveElement(Sciter::ElementUID* uid) const;
			void AcceptElement(Sciter::ElementUID*& uid, const Sciter::Element& element);

		public:
			SciterEvent(Sciter::Host& host)
				:m_Host(&host)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<SciterEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const noexcept override
			{
				return EventCategory::UI;
			}

			//SciterEvent
			Sciter::Host& GetHost() const
			{
				return *m_Host;
			}

			Sciter::Element GetElement() const
			{
				return RetrieveElement(m_Element);
			}
			void SetElement(const Sciter::Element& element)
			{
				AcceptElement(m_Element, element);
			}
	
			Sciter::Element GetTargetElement() const
			{
				return RetrieveElement(m_TargetElement);
			}
			void SetTargetElement(const Sciter::Element& element)
			{
				AcceptElement(m_TargetElement, element);
			}
	};
}

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Attached);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Detached);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, EngineCreated);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, EngineDestroyed);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Gesture);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, Exchange);

	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, ContentLoaded);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, TIScriptCall);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SciterEvent, ScriptingCall);
}
