#pragma once
#include "Common.h"
#include "EventID.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/General/DateTime/TimeSpan.h"
#include "kxf/UI/WidgetID.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class EvtHandler;
}

namespace kxf
{
	class KX_API Event: public RTTI::Interface<Event>
	{
		KxDeclareIID(Event, {0x61df394f, 0x8d5c, 0x43ef, {0xbd, 0x9f, 0xfb, 0xeb, 0xbf, 0x1e, 0x97, 0xa5}});

		friend class EvtHandler;

		public:
			static inline const EventTag<Event> EvtNull = 0;
			static inline const EventTag<Event> EvtAny = -1;

		private:
			EventID m_EventID;
			EvtHandler* m_EventSource = nullptr;
			FlagSet<EventCategory> m_Category;
			UniversallyUniqueID m_UniqueID;
			TimeSpan m_Timestamp;

			bool m_IsAllowed = true;
			bool m_IsSkipped = false;
			bool m_ExceptionThrown = false;

			// Initially false but becomes true as soon as 'WasProcessed' is called for the first time,
			// as this is done only by 'DoProcessEvent' it explains the variable name: it becomes true
			// after 'DoProcessEvent' was called at least once for this event.
			mutable bool m_WasProcessed = false;

			// This one is initially false too, but can be set to true to indicate that
			// the event will be passed to another handler if it's not processed in this one.
			mutable bool m_WillBeProcessedAgain = false;

		private:
			bool WasProcessed() const
			{
				if (m_WasProcessed)
				{
					return true;
				}

				m_WasProcessed = true;
				return false;
			}
			bool WillBeProcessedAgain() const
			{
				if (m_WillBeProcessedAgain)
				{
					m_WillBeProcessedAgain = false;
					return true;
				}
				return false;
			}

		public:
			Event() = default;
			Event(const Event&) noexcept = default;
			Event(Event&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			virtual std::unique_ptr<Event> Move() noexcept
			{
				return std::make_unique<Event>(std::move(*this));
			}

			EventID GetEventID() const
			{
				return m_EventID;
			}
			TimeSpan GetTimestamp() const
			{
				return m_Timestamp;
			}
			UniversallyUniqueID GetUniqueID() const
			{
				return m_UniqueID;
			}

			EvtHandler* GetEventSource() const
			{
				return m_EventSource;
			}
			void SetEventSource(EvtHandler* evtHandler)
			{
				m_EventSource = evtHandler;
			}
			
			FlagSet<EventCategory> GetEventCategory() const
			{
				return m_Category;
			}
			void SetEventCategory(FlagSet<EventCategory> category)
			{
				m_Category = category;
			}

			bool IsSkipped() const
			{
				return m_IsSkipped;
			}
			void Skip(bool skip = true)
			{
				m_IsSkipped = skip;
			}

			bool IsAllowed() const
			{
				return m_IsAllowed;
			}
			void Allow(bool allow = true)
			{
				m_IsAllowed = allow;
			}

			bool WasProcessed() const
			{
				return m_WasProcessed;
			}
			bool ExceptionThrown() const
			{
				return m_ExceptionThrown;
			}

		public:
			Event& operator=(const Event&) noexcept = default;
			Event& operator=(Event&& other) noexcept
			{
				m_EventID = std::move(other.m_EventID);
				m_EventSource = Utility::ExchangeResetAndReturn(other.m_EventSource, nullptr);
				m_Category = Utility::ExchangeResetAndReturn(other.m_Category, EventCategory::None);
				m_IsAllowed = Utility::ExchangeResetAndReturn(other.m_IsAllowed, true);
				m_IsSkipped = Utility::ExchangeResetAndReturn(other.m_IsSkipped, false);
				m_ExceptionThrown = Utility::ExchangeResetAndReturn(other.m_ExceptionThrown, false);
				m_WasProcessed = Utility::ExchangeResetAndReturn(other.m_WasProcessed, false);
				m_WillBeProcessedAgain = Utility::ExchangeResetAndReturn(other.m_WillBeProcessedAgain, false);

				return *this;
			}
	};
}

namespace kxf
{
	class KX_API ICommandEvent: public RTTI::Interface<ICommandEvent>
	{
		KxDeclareIID(ICommandEvent, {0x4552fa23, 0xb7da, 0x44c5, {0x86, 0x93, 0x30, 0x40, 0x87, 0x31, 0x33, 0x72}});

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
			virtual ~ICommandEvent() = default;

		public:
			virtual WidgetID GetCommandID() const = 0;
			virtual void SetCommandID(WidgetID id) = 0;

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

	class IAsyncEvent: public RTTI::Interface<IAsyncEvent>
	{
		KxDeclareIID(IAsyncEvent, {0xb0046b41, 0xecd9, 0x4b46, {0xb1, 0xb5, 0xd6, 0x7d, 0xa, 0x77, 0x5c, 0xbe}});

		public:
			virtual ~IAsyncEvent() = default;

		public:
			virtual void Execute() = 0;
	};
}

namespace kxf
{
	class KX_API CommandEvent: public RTTI::ImplementInterface<CommandEvent, Event, ICommandEvent>
	{
		private:
			WidgetID m_CommandID;
			size_t m_PropagationLevel = PropagationLevel::Max;

			String m_String;
			int64_t m_Int = 0;
			int64_t m_ExtraInt = 0;

		public:
			CommandEvent() = default;
			CommandEvent(const CommandEvent&) = default;
			CommandEvent(CommandEvent&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// Event
			std::unique_ptr<Event> Move() noexcept override
			{
				return std::make_unique<CommandEvent>(std::move(*this));
			}

			// ICommandEvent
			WidgetID GetCommandID() const override
			{
				return m_CommandID;
			}
			void SetCommandID(WidgetID id) override
			{
				m_CommandID = id;
			}

			size_t StopPropagation() override
			{
				return Utility::ExchangeResetAndReturn(m_PropagationLevel, PropagationLevel::None);
			}
			void ResumePropagation(size_t level) override
			{
				m_PropagationLevel = level;
			}
			bool ShouldPropagate() const override
			{
				return m_PropagationLevel != PropagationLevel::None;
			}
			
			String GetString() const override
			{
				return m_String;
			}
			void SetString(const String& value) override
			{
				m_String = value;
			}
			void SetString(String&& value)
			{
				m_String = std::move(value);
			}

			int64_t GetInt() const override
			{
				return m_Int;
			}
			void SetInt(int64_t value) override
			{
				m_Int = value;
			}

			int64_t GetExtraInt() const override
			{
				return m_ExtraInt;
			}
			void SetExtraInt(int64_t value) override
			{
				m_ExtraInt = value;
			}

		public:
			CommandEvent& operator=(const CommandEvent&) = default;
			CommandEvent& operator=(CommandEvent&& other) noexcept
			{
				m_CommandID = Utility::ExchangeResetAndReturn(other.m_CommandID, wxID_NONE);
				m_PropagationLevel = Utility::ExchangeResetAndReturn(other.m_PropagationLevel, PropagationLevel::Max);
				m_String = std::move(other.m_String);

				return *this;
			}
	};
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
