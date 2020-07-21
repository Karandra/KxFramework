#pragma once
#include "Common.h"
#include "IEvent.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class KX_API CommonEvent: public RTTI::ImplementInterface<CommonEvent, IEvent>
	{
		private:
			EventID m_EventID;
			EvtHandler* m_EventSource = nullptr;
			UniversallyUniqueID m_UniqueID;
			TimeSpan m_Timestamp;

			bool m_IsAllowed = true;
			bool m_IsSkipped = false;

			// Initially false but becomes true as soon as 'WasProcessed' is called for the first time,
			// as this is done only by 'DoProcessEvent' it explains the variable name: it becomes true
			// after 'DoProcessEvent' was called at least once for this event.
			mutable bool m_WasProcessed = false;

			// This one is initially false too, but can be set to true to indicate that
			// the event will be passed to another handler if it's not processed in this one.
			mutable bool m_WillBeProcessedAgain = false;

		private:
			bool WasProcessed() const override
			{
				if (m_WasProcessed)
				{
					return true;
				}

				m_WasProcessed = true;
				return false;
			}
			bool WillBeProcessedAgain() const override
			{
				if (m_WillBeProcessedAgain)
				{
					m_WillBeProcessedAgain = false;
					return true;
				}
				return false;
			}

			void OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid) override
			{
				if (eventID && !m_EventID)
				{
					m_EventID = eventID;
				}
				if (!m_UniqueID)
				{
					m_UniqueID = std::move(uuid);
				}
				if (!m_Timestamp.IsPositive())
				{
					m_Timestamp = TimeSpan::Now(SteadyClock());
				}
			}

		public:
			CommonEvent() = default;
			CommonEvent(const CommonEvent&) noexcept = default;
			CommonEvent(CommonEvent&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<CommonEvent>(std::move(*this));
			}

			EventID GetEventID() const override
			{
				return m_EventID;
			}
			TimeSpan GetTimestamp() const override
			{
				return m_Timestamp;
			}
			UniversallyUniqueID GetUniqueID() const override
			{
				return m_UniqueID;
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return EventCategory::None;
			}

			EvtHandler* GetEventSource() const override
			{
				return m_EventSource;
			}
			void SetEventSource(EvtHandler* evtHandler) override
			{
				m_EventSource = evtHandler;
			}
			
			bool IsSkipped() const override
			{
				return m_IsSkipped;
			}
			void Skip(bool skip = true) override
			{
				m_IsSkipped = skip;
			}

			bool IsAllowed() const override
			{
				return m_IsAllowed;
			}
			void Allow(bool allow = true) override
			{
				m_IsAllowed = allow;
			}

		public:
			CommonEvent& operator=(const CommonEvent&) noexcept = default;
			CommonEvent& operator=(CommonEvent&& other) noexcept
			{
				m_EventID = std::move(other.m_EventID);
				m_EventSource = Utility::ExchangeResetAndReturn(other.m_EventSource, nullptr);
				m_IsAllowed = Utility::ExchangeResetAndReturn(other.m_IsAllowed, true);
				m_IsSkipped = Utility::ExchangeResetAndReturn(other.m_IsSkipped, false);
				m_WasProcessed = Utility::ExchangeResetAndReturn(other.m_WasProcessed, false);
				m_WillBeProcessedAgain = Utility::ExchangeResetAndReturn(other.m_WillBeProcessedAgain, false);

				return *this;
			}
	};

	class KX_API WidgetEvent: public RTTI::ImplementInterface<WidgetEvent, CommonEvent, IWidgetEvent>
	{
		private:
			WidgetID m_WidgetID;
			size_t m_PropagationLevel = PropagationLevel::Max;

			String m_String;
			int64_t m_Int = 0;
			int64_t m_ExtraInt = 0;

		public:
			WidgetEvent() = default;
			WidgetEvent(const WidgetEvent&) = default;
			WidgetEvent(WidgetEvent&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				// This function is used to selectively process events in 'IEventLoop::YieldFor'
				// It returns 'EventCategory::UI' because 'WidgetEvent' is an UI event.
				return EventCategory::UI;
			}

			// IWidgetEvent
			WidgetID GetWidgetID() const override
			{
				return m_WidgetID;
			}
			void SetWidgetID(WidgetID id) override
			{
				m_WidgetID = id;
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
			WidgetEvent& operator=(const WidgetEvent&) = default;
			WidgetEvent& operator=(WidgetEvent&& other) noexcept
			{
				m_WidgetID = Utility::ExchangeResetAndReturn(other.m_WidgetID, wxID_NONE);
				m_PropagationLevel = Utility::ExchangeResetAndReturn(other.m_PropagationLevel, PropagationLevel::Max);
				m_String = std::move(other.m_String);

				return *this;
			}
	};
}
