#pragma once
#include "EventID.h"
#include "IEvent.h"
#include "IEventExecutor.h"
#include "kxf/General/LocallyUniqueID.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/WithOptionalOwnership.h"

namespace kxf::EventSystem
{
	class EventItem final
	{
		private:
			IEventExecutor* m_Executor = nullptr;
			bool m_ExecutorOwn = false;

			EventID m_EventID;
			LocallyUniqueID m_BindSlot;
			FlagSet<EventFlag> m_Flags;

		private:
			void Destroy() noexcept
			{
				if (m_ExecutorOwn)
				{
					delete m_Executor;
				}
				m_Executor = nullptr;
				m_ExecutorOwn = false;
			}

		public:
			EventItem() noexcept = default;
			EventItem(EventID eventID, std::unique_ptr<IEventExecutor> executor) noexcept
				:m_Executor(executor.release()), m_EventID(eventID), m_ExecutorOwn(true)
			{
			}
			EventItem(EventID eventID, IEventExecutor& executor) noexcept
				:m_Executor(&executor), m_EventID(eventID), m_ExecutorOwn(false)
			{
			}
			EventItem(EventItem&& other) noexcept
			{
				*this = std::move(other);
			}
			EventItem(const EventItem&) = delete;
			~EventItem() noexcept
			{
				Destroy();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Executor == nullptr || m_EventID.IsNull();
			}
			bool IsSameAs(const EventItem& other) const noexcept
			{
				if (this != &other)
				{
					const bool sameID = m_EventID == other.m_EventID;
					return sameID && (m_Executor == other.m_Executor || (m_Executor && other.m_Executor && m_Executor->IsSameAs(*other.m_Executor)));
				}
				return true;
			}

			const IEventExecutor* GetExecutor() const& noexcept
			{
				return m_Executor;
			}
			IEventExecutor* GetExecutor() & noexcept
			{
				return m_Executor;
			}
			Utility::WithOptionalOwnership<IEventExecutor> GetExecutor() &&
			{
				Utility::WithOptionalOwnership<IEventExecutor> ptr;
				if (m_Executor)
				{
					if (m_ExecutorOwn)
					{
						ptr.Assign(std::unique_ptr<IEventExecutor>(Utility::ExchangeResetAndReturn(m_Executor, nullptr)));
						m_ExecutorOwn = false;
					}
					else
					{
						ptr.Assign(*Utility::ExchangeResetAndReturn(m_Executor, nullptr));
					}
				}
				return ptr;
			}
			void SetExecutor(std::unique_ptr<IEventExecutor> executor)
			{
				Destroy();
				m_Executor = executor.release();
				m_ExecutorOwn = true;
			}
			void SetExecutor(IEventExecutor& executor)
			{
				Destroy();
				m_Executor = &executor;
				m_ExecutorOwn = false;
			}

			EventID GetEventID() const noexcept
			{
				return m_EventID;
			}
			void SetEventID(EventID eventID)
			{
				m_EventID = eventID;
			}

			FlagSet<EventFlag> GetFlags() const noexcept
			{
				return m_Flags;
			}
			void SetFlags(FlagSet<EventFlag> flags) noexcept
			{
				m_Flags = flags;
			}

			LocallyUniqueID GetBindSlot() const noexcept
			{
				return m_BindSlot;
			}
			void SetBindSlot(LocallyUniqueID cookie) noexcept
			{
				m_BindSlot = std::move(cookie);
			}

		public:
			EventItem& operator=(EventItem&& other) noexcept
			{
				Destroy();

				m_Executor = Utility::ExchangeResetAndReturn(other.m_Executor, nullptr);
				m_ExecutorOwn = Utility::ExchangeResetAndReturn(other.m_ExecutorOwn, false);
				m_EventID = std::move(other.m_EventID);
				m_BindSlot = std::move(other.m_BindSlot);

				return *this;
			}
			EventItem& operator=(const EventItem&) = delete;

			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
