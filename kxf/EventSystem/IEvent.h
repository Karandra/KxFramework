#pragma once
#include "Common.h"
#include "EventID.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/General/DateTime/TimeSpan.h"
#include "kxf/UI/WidgetID.h"

namespace kxf
{
	class IEvtHandler;
}
namespace kxf::EventSystem
{
	class EventAccessor;
}

namespace kxf
{
	class KX_API IEvent: public RTTI::Interface<IEvent>
	{
		KxDeclareIID(IEvent, {0x61df394f, 0x8d5c, 0x43ef, {0xbd, 0x9f, 0xfb, 0xeb, 0xbf, 0x1e, 0x97, 0xa5}});

		friend class EventSystem::EventAccessor;

		public:
			static inline const EventTag<IEvent> EvtNull = 0;
			static inline const EventTag<IEvent> EvtAny = -1;

		private:
			virtual bool WasProcessed() const = 0;
			virtual bool WillBeProcessedAgain() const = 0;

			virtual void OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid) = 0;

		public:
			virtual ~IEvent() = default;

		public:
			virtual std::unique_ptr<IEvent> Move() noexcept = 0;

			virtual EventID GetEventID() const = 0;
			virtual TimeSpan GetTimestamp() const = 0;
			virtual UniversallyUniqueID GetUniqueID() const = 0;
			virtual FlagSet<EventCategory> GetEventCategory() const = 0;

			virtual IEvtHandler* GetEventSource() const = 0;
			virtual void SetEventSource(IEvtHandler* evtHandler) = 0;

			virtual bool IsSkipped() const = 0;
			virtual void Skip(bool skip = true) = 0;

			virtual bool IsAllowed() const = 0;
			virtual void Allow(bool allow = true) = 0;
	};

	class KX_API IWidgetEvent: public RTTI::Interface<IWidgetEvent>
	{
		KxDeclareIID(IWidgetEvent, {0x4552fa23, 0xb7da, 0x44c5, {0x86, 0x93, 0x30, 0x40, 0x87, 0x31, 0x33, 0x72}});

		public:
			struct PropagationLevel final
			{
				// Don't propagate it at all
				static constexpr size_t None = 0;

				// Propagate it until it is processed
				static constexpr size_t Max = std::numeric_limits<uint32_t>::max();

				PropagationLevel() noexcept = delete;
			};

		public:
			virtual ~IWidgetEvent() = default;

		public:
			virtual WidgetID GetWidgetID() const = 0;
			virtual void SetWidgetID(WidgetID id) = 0;

			virtual size_t StopPropagation() = 0;
			virtual void ResumePropagation(size_t level) = 0;
			virtual bool ShouldPropagate() const = 0;

			virtual String GetString() const = 0;
			virtual void SetString(const String& value) = 0;

			virtual int64_t GetInt() const = 0;
			virtual void SetInt(int64_t value) = 0;

			virtual int64_t GetExtraInt() const = 0;
			virtual void SetExtraInt(int64_t value) = 0;
	};

	class IIndirectInvocationEvent: public RTTI::Interface<IIndirectInvocationEvent>
	{
		KxDeclareIID(IIndirectInvocationEvent, {0xb0046b41, 0xecd9, 0x4b46, {0xb1, 0xb5, 0xd6, 0x7d, 0xa, 0x77, 0x5c, 0xbe}});

		public:
			static inline const EventTag<IEvent> EvtIndirectInvocation = -2;

		public:
			virtual ~IIndirectInvocationEvent() = default;

		public:
			virtual void Execute() = 0;
	};

	class ISignalInvocationEvent: public RTTI::Interface<ISignalInvocationEvent>
	{
		KxDeclareIID(ISignalInvocationEvent, {0xb3ad1ef2, 0x96ee, 0x4ebf, {0x93, 0xac, 0x6, 0xf8, 0x5b, 0x2e, 0xe5, 0xc5}});

		public:
			virtual ~ISignalInvocationEvent() = default;

		public:
			virtual bool GetParameters(void* parameters) = 0;

			virtual void TakeResult(void* value) = 0;
			virtual void PutResult(void* value) = 0;
	};

	template<class TFunc, class = std::enable_if_t<std::is_member_function_pointer_v<TFunc>>>
	EventTag(TFunc func) -> EventTag<ISignalInvocationEvent>;
}

// Event declaration macros

// For global event IDs
#define KxEVENT_DECLARE_GLOBAL(type, name)						extern const KX_API kxf::EventTag<type> KxEVT_##name
#define KxEVENT_DEFINE_GLOBAL(type, name)						const kxf::EventTag<type> KxEVT_##name = kxf::EventSystem::NewSimpleEventID()
#define KxEVENT_DEFINE_GLOBAL_AS(type, name, other)				const kxf::EventTag<type> KxEVT_##name = static_cast<kxf::EventID>(other)

// For local scope (namespace, function, etc)
#define KxEVENT_DECLARE_LOCAL(type, name)						extern KX_API const kxf::EventTag<type> Evt##name
#define KxEVENT_DEFINE_LOCAL(type, name)						const kxf::EventTag<type> Evt##name = kxf::EventSystem::NewSimpleEventID()
#define KxEVENT_DEFINE_LOCAL_AS(type, name, other)				const kxf::EventTag<type> Evt##name = static_cast<kxf::EventID>(other)

// As static member of an event class
#define KxEVENT_DECLARE_MEMBER(type, name)						static const kxf::EventTag<type> Evt##name
#define KxEVENT_DEFINE_MEMBER(type, name)						const kxf::EventTag<type> type::Evt##name = kxf::EventSystem::NewSimpleEventID()
#define KxEVENT_DEFINE_MEMBER_AS(type, name, other)				const kxf::EventTag<type> type::Evt##name = static_cast<kxf::EventID>(other)
#define KxEVENT_DECLARE_ALIAS_TO_MEMBER(type, name)				inline const kxf::EventTag<type> Evt##name = type::Evt##name
#define KxEVENT_DECLARE_ALIAS_TO_MEMBER_AS(type, name, other)	inline const kxf::EventTag<type> Evt##name = type::Evt##other

// As inline static member of an event class
#define KxEVENT_MEMBER(type, name)								inline static const kxf::EventTag<type> Evt##name = kxf::EventSystem::NewSimpleEventID()
#define KxEVENT_MEMBER_AS(type, name, other)					inline static const kxf::EventTag<type> Evt##name = static_cast<kxf::EventID>(other)
