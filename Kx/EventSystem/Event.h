#pragma once
#include "Common.h"
#include <functional>
class KxEvtHandler;
class KxCoreApplication;
class KxIndirectCallEvent;

class KX_API KxEvent: public wxEvent
{
	public:
		static inline const KxEventTag<KxEvent> EvtNull = wxEVT_NULL;
		static inline const KxEventTag<KxEvent> EvtAny = wxEVT_ANY;

	public:
		static KxEventID NewEventID() noexcept
		{
			return ::wxNewEventType();
		}

	public:
		KxEvent(KxEventID eventID = EvtNull) noexcept
			:wxEvent(eventID)
		{
		}
		virtual ~KxEvent() = default;

	public:
		KxEvent* Clone() const override
		{
			return new KxEvent(*this);
		}
};

// Event declaration macros

// For global event IDs
#define KxEVENT_DECLARE_GLOBAL(name, type)				extern const KX_API KxEventTag<type> KxEVT_##name
#define KxEVENT_DEFINE_GLOBAL(name, type)				const KxEventTag<type> KxEVT_##name = KxEvent::NewEventID()
#define KxEVENT_DEFINE_GLOBAL_AS(name, type, other)		const KxEventTag<type> KxEVT_##name = static_cast<KxEventID>(other)

// For local scope (namespace, function, etc)
#define KxEVENT_DECLARE_LOCAL(name, type)				extern KX_API const KxEventTag<type> Evt##name
#define KxEVENT_DEFINE_LOCAL(name, type)				const KxEventTag<type> Evt##name = KxEvent::NewEventID()
#define KxEVENT_DEFINE_LOCAL_AS(name, type, other)		const KxEventTag<type> Evt##name = static_cast<KxEventID>(other)

// As static member of an event class
#define KxEVENT_DECLARE_MEMBER(type, name)				static const KxEventTag<type> Evt##name
#define KxEVENT_DEFINE_MEMBER(type, name)				const KxEventTag<type> type::Evt##name = KxEvent::NewEventID()
#define KxEVENT_DEFINE_MEMBER_AS(type, name, other)		const KxEventTag<type> type::Evt##name = static_cast<KxEventID>(other)

// As inline static member of an event class
#define KxEVENT_MEMBER(type, name)						inline static const KxEventTag<type> Evt##name = KxEvent::NewEventID()
#define KxEVENT_MEMBER_AS(type, name, other)			inline static const KxEventTag<type> Evt##name = static_cast<KxEventID>(other)
