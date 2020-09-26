#pragma once
#include "EventID.h"
#include "IEvent.h"
#include "IEventExecutor.h"
#include "kxf/General/LocallyUniqueID.h"
#include "kxf/Utility/Common.h"
#include "kxf/General/OptionalPtr.h"

namespace kxf::EventSystem
{
	class EventItem final
	{
		private:
			optional_ptr<IEventExecutor> m_Executor;
			EventID m_EventID;
			LocallyUniqueID m_BindSlot;
			FlagSet<BindEventFlag> m_Flags;

		public:
			EventItem() noexcept = default;
			EventItem(EventID eventID, std::unique_ptr<IEventExecutor> executor) noexcept
				:m_Executor(std::move(executor)), m_EventID(eventID)
			{
			}
			EventItem(EventID eventID, IEventExecutor& executor) noexcept
				:m_Executor(executor), m_EventID(eventID)
			{
			}
			EventItem(EventItem&&) noexcept = default;
			EventItem(const EventItem&) = delete;

		public:
			bool IsNull() const noexcept
			{
				return m_Executor.is_null() || m_EventID.IsNull();
			}
			bool IsSameAs(const EventItem& other) const noexcept
			{
				if (this != &other)
				{
					const bool sameID = m_EventID == other.m_EventID;
					return sameID && (m_Executor.is_same_as(other.m_Executor) || (m_Executor && other.m_Executor && m_Executor->IsSameAs(*other.m_Executor)));
				}
				return true;
			}
			bool IsSameEventID(const EventID& eventID) const noexcept
			{
				return m_EventID == eventID;
			}

			const IEventExecutor* GetExecutor() const& noexcept
			{
				return m_Executor.get();
			}
			IEventExecutor* GetExecutor() & noexcept
			{
				return m_Executor.get();
			}
			optional_ptr<IEventExecutor> GetExecutor() &&
			{
				return std::move(m_Executor);
			}
			void SetExecutor(std::unique_ptr<IEventExecutor> executor)
			{
				m_Executor = std::move(executor);
			}
			void SetExecutor(IEventExecutor& executor)
			{
				m_Executor = executor;
			}

			EventID GetEventID() const noexcept
			{
				return m_EventID;
			}
			void SetEventID(EventID eventID)
			{
				m_EventID = eventID;
			}

			FlagSet<BindEventFlag> GetFlags() const noexcept
			{
				return m_Flags;
			}
			void SetFlags(FlagSet<BindEventFlag> flags) noexcept
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
			EventItem& operator=(EventItem&&) noexcept = default;
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
