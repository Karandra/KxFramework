#pragma once
#include "Common.h"
#include <functional>

namespace kxf
{
	class EvtHandler;
	class IIndirectCallEvent;
}

namespace kxf
{
	class KX_API Event: public wxEvent
	{
		public:
			static inline const EventTag<Event> EvtNull = wxEVT_NULL;
			static inline const EventTag<Event> EvtAny = wxEVT_ANY;

		public:
			static EventID NewEventID() noexcept
			{
				return ::wxNewEventType();
			}

		public:
			Event(EventID eventID = EvtNull) noexcept
				:wxEvent(eventID)
			{
			}
			virtual ~Event() = default;

		public:
			Event* Clone() const override
			{
				return new Event(*this);
			}
	};
}

// Event declaration macros

// For global event IDs
#define KxEVENT_DECLARE_GLOBAL(type, name)						extern const KX_API kxf::EventTag<type> KxEVT_##name
#define KxEVENT_DEFINE_GLOBAL(type, name)						const kxf::EventTag<type> KxEVT_##name = kxf::Event::NewEventID()
#define KxEVENT_DEFINE_GLOBAL_AS(type, name, other)				const kxf::EventTag<type> KxEVT_##name = static_cast<kxf::EventID>(other)

// For local scope (namespace, function, etc)
#define KxEVENT_DECLARE_LOCAL(type, name)						extern KX_API const kxf::EventTag<type> Evt##name
#define KxEVENT_DEFINE_LOCAL(type, name)						const kxf::EventTag<type> Evt##name = kxf::Event::NewEventID()
#define KxEVENT_DEFINE_LOCAL_AS(type, name, other)				const kxf::EventTag<type> Evt##name = static_cast<kxf::EventID>(other)

// As static member of an event class
#define KxEVENT_DECLARE_MEMBER(type, name)						static const kxf::EventTag<type> Evt##name
#define KxEVENT_DEFINE_MEMBER(type, name)						const kxf::EventTag<type> type::Evt##name = kxf::Event::NewEventID()
#define KxEVENT_DEFINE_MEMBER_AS(type, name, other)				const kxf::EventTag<type> type::Evt##name = static_cast<kxf::EventID>(other)
#define KxEVENT_DECLARE_ALIAS_TO_MEMBER(type, name)				inline const kxf::EventTag<type> Evt##name = type::Evt##name
#define KxEVENT_DECLARE_ALIAS_TO_MEMBER_AS(type, name, other)	inline const kxf::EventTag<type> Evt##name = type::Evt##other

// As inline static member of an event class
#define KxEVENT_MEMBER(type, name)								inline static const kxf::EventTag<type> Evt##name = kxf::Event::NewEventID()
#define KxEVENT_MEMBER_AS(type, name, other)					inline static const kxf::EventTag<type> Evt##name = static_cast<kxf::EventID>(other)
