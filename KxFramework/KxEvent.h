#pragma once
#include "KxFramework/KxFramework.h"

// Event declaration macros

// For global event IDs
#define KxEVENT_DECLARE_GLOBAL(name, type)					extern const KX_API wxEventTypeTag<type> KxEVT_##name
#define KxEVENT_DECLARE_GLOBAL_AS(name, type, other)		extern const KX_API wxEventTypeTag<type> KxEVT_##name = (other)
#define KxEVENT_DEFINE_GLOBAL(name, type)					const wxEventTypeTag<type> KxEVT_##name = wxNewEventType()

// For local scope (namespace, function, etc)
#define KxEVENT_DECLARE_LOCAL(name, type)					extern KX_API const wxEventTypeTag<type> Evt##name
#define KxEVENT_DEFINE_LOCAL(name, type)					const wxEventTypeTag<type> Evt##name = wxNewEventType()
#define KxEVENT_DEFINE_LOCAL_AS(name, type, other)			const wxEventTypeTag<type> Evt##name = (other)

// As static member of an event class
#define KxEVENT_DECLARE_MEMBER(type, name)					static const wxEventTypeTag<type> Evt##name
#define KxEVENT_DEFINE_MEMBER(type, name)					const wxEventTypeTag<type> type::Evt##name = wxNewEventType()
#define KxEVENT_DEFINE_MEMBER_AS(type, name, other)			const wxEventTypeTag<type> type::Evt##name = (other)

// As inline static member of an event class
#define KxEVENT_MEMBER(type, name)							inline static const wxEventTypeTag<type> Evt##name = wxNewEventType()
#define KxEVENT_MEMBER_AS(type, name, other)				inline static const wxEventTypeTag<type> Evt##name = (other)
