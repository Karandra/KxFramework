#pragma once
#include "Event.h"

namespace kxf
{
	class IEvtHandler;
}

namespace kxf::EventSystem::Private
{
	template<class T, class TEvent>
	class EventBuilderCRTP
	{
		private:
			T& Self()
			{
				return static_cast<T&>(*this);
			}

		public:
			~EventBuilderCRTP()
			{
				if (!Self().m_IsSent)
				{
					Self().Execute();
				}
			}

		public:
			template<class TFunc>
			T& Setup(TFunc&& func)
			{
				std::invoke(func, static_cast<TEvent&>(*Self().m_Event));
				return Self();
			}

			T& SetSourceToSelf()
			{
				Self().m_Event->SetEventSource(Self().m_EvtHandler);
				return Self();
			}
			T& SetSource(IEvtHandler& source)
			{
				Self().m_Event->SetEventSource(&source);
				return Self();
			}
	};
}

namespace kxf::EventSystem
{
	class KX_API EventBuilderBase
	{
		protected:
			IEvtHandler* m_EvtHandler = nullptr;
			IEvent* m_Event = nullptr;
			EventID m_EventID;

			bool m_IsAsync = false;
			bool m_IsSent = false;
			bool m_IsSkipped = false;
			bool m_IsAllowed = true;
			bool m_IsProcessed = false;

		private:
			void Destroy() noexcept;
			void Move(EventBuilderBase&& other) noexcept;

			void SendEvent(bool locally, bool safely, UniversallyUniqueID uuid);

		protected:
			void QueueEvent(UniversallyUniqueID id = {})
			{
				SendEvent(false, false, std::move(id));
			}
			void ProcessEvent()
			{
				SendEvent(false, false, {});
			}
			void ProcessEventLocally()
			{
				SendEvent(true, false, {});
			}
			void ProcessEventSafely()
			{
				SendEvent(false, true, {});
			}

		private:
			EventBuilderBase() = default;

		protected:
			EventBuilderBase(IEvtHandler& evtHandler, std::unique_ptr<IEvent> event, const EventID& eventID = {}) noexcept
				:m_EvtHandler(&evtHandler), m_Event(event.release()), m_EventID(eventID), m_IsAsync(true)
			{
			}
			EventBuilderBase(IEvtHandler& evtHandler, IEvent& event, const EventID& eventID = {}) noexcept
				:m_EvtHandler(&evtHandler), m_Event(&event), m_EventID(eventID), m_IsAsync(false)
			{
			}
			EventBuilderBase(EventBuilderBase&& other) noexcept
			{
				Move(std::move(other));
			}
			EventBuilderBase(const EventBuilderBase&) = delete;
			virtual ~EventBuilderBase()
			{
				Destroy();
			}

		public:
			bool IsAsync() const noexcept
			{
				return m_IsAsync;
			}
			bool IsSkipped() const noexcept
			{
				return m_IsSkipped;
			}
			bool IsAllowed() const noexcept
			{
				return m_IsAllowed;
			}
			bool IsProcessed() const noexcept
			{
				return m_IsProcessed;
			}

		public:
			EventBuilderBase& operator=(EventBuilderBase&& other) noexcept
			{
				Move(std::move(other));
				return *this;
			}
			EventBuilderBase& operator=(const EventBuilderBase&) = delete;
	};
}

namespace kxf::EventSystem
{
	template<class TEvent>
	class DirectEventBuilder: public EventBuilderBase, public Private::EventBuilderCRTP<DirectEventBuilder<TEvent>, TEvent>
	{
		friend class Private::EventBuilderCRTP<DirectEventBuilder, TEvent>;

		private:
			TEvent m_EventInstance;

		public:
			DirectEventBuilder(IEvtHandler& evtHandler, TEvent event, const EventID& eventID)
				:EventBuilderBase(evtHandler, m_EventInstance, eventID), m_EventInstance(std::move(event))
			{
			}
			DirectEventBuilder(DirectEventBuilder&& other)
				:EventBuilderBase(std::move(other))
			{
			}
			DirectEventBuilder(const DirectEventBuilder&) = delete;

		public:
			DirectEventBuilder& Execute()
			{
				EventBuilderBase::ProcessEvent();
				return *this;
			}
			DirectEventBuilder& ExecuteSafely()
			{
				EventBuilderBase::ProcessEventSafely();
				return *this;
			}
			DirectEventBuilder& ExecuteLocally()
			{
				EventBuilderBase::ProcessEventLocally();
				return *this;
			}

			TEvent& GetEvent() noexcept
			{
				return m_EventInstance;
			}

		public:
			DirectEventBuilder& operator=(DirectEventBuilder&& other) noexcept
			{
				static_cast<EventBuilderBase&>(*this) = std::move(other);
				return *this;
			}
			DirectEventBuilder& operator=(const DirectEventBuilder&) = delete;
	};

	template<class TEvent>
	class QueuedEventBuilder: public EventBuilderBase, public Private::EventBuilderCRTP<QueuedEventBuilder<TEvent>, TEvent>
	{
		friend class Private::EventBuilderCRTP<QueuedEventBuilder, TEvent>;

		public:
			QueuedEventBuilder(IEvtHandler& evtHandler, std::unique_ptr<TEvent> event, const EventID& eventID)
				:EventBuilderBase(evtHandler, std::move(event), eventID)
			{
			}
			QueuedEventBuilder(QueuedEventBuilder&& other)
				:EventBuilderBase(std::move(other))
			{
			}
			QueuedEventBuilder(const QueuedEventBuilder&) = delete;

		public:
			QueuedEventBuilder& Execute()
			{
				EventBuilderBase::QueueEvent();
				return *this;
			}
			QueuedEventBuilder& ExecuteUnique(UniversallyUniqueID id)
			{
				EventBuilderBase::QueueEvent(std::move(id));
				return *this;
			}

			TEvent* GetEvent() noexcept
			{
				return m_Event;
			}

		public:
			QueuedEventBuilder& operator=(QueuedEventBuilder&& other) noexcept
			{
				static_cast<EventBuilderBase&>(*this) = std::move(other);
				return *this;
			}
			QueuedEventBuilder& operator=(const QueuedEventBuilder&) = delete;
	};
}
