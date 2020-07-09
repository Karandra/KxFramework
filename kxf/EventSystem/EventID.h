#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/UniversallyUniqueID.h"
#include <variant>

namespace kxf
{
	class KX_API EventID final
	{
		private:
			std::variant<int64_t, UniversallyUniqueID, String> m_ID;

		public:
			EventID() noexcept = default;
			EventID(int64_t id) noexcept
				:m_ID(id)
			{
			}
			EventID(UniversallyUniqueID id) noexcept
				:m_ID(std::move(id))
			{
			}
			EventID(String id) noexcept
				:m_ID(std::move(id))
			{
			}
			
			template<class T>
			EventID(const wxEventTypeTag<T>& eventTag) noexcept
				:m_ID(static_cast<wxEventType>(eventTag))
			{
			}
			
			EventID(EventID&& other) noexcept
				:m_ID(std::move(other.m_ID))
			{
			}
			EventID(const EventID&) = default;

		public:
			bool IsNull() const noexcept;
			size_t GetHash() const noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const EventID& other) const noexcept
			{
				return m_ID == other.m_ID;
			}
			bool operator!=(const EventID& other) const noexcept
			{
				return m_ID != other.m_ID;
			}

			EventID& operator=(EventID&& other) noexcept
			{
				m_ID = std::move(other.m_ID);
				return *this;
			}
			EventID& operator=(const EventID&) = default;
	};
}

namespace kxf
{
	template<class T>
	class EventTag final
	{
		private:
			EventID m_ID;

		public:
			EventTag() noexcept = default;
			EventTag(EventID id) noexcept
				:m_ID(std::move(id))
			{
			}
			EventTag(EventTag&& other) noexcept
				:m_ID(std::move(other))
			{
			}
			EventTag(const EventTag&) = default;

			template<class T, class = std::enable_if_t<std::is_nothrow_constructible_v<EventID, T>>>
			EventTag(T&& arg) noexcept
				:m_ID(std::forward<T>(arg))
			{
			}

		public:
			bool IsNull() const noexcept
			{
				m_ID.IsNull();
			}

			const EventID& operator*() const noexcept
			{
				return m_ID;
			}
			EventID& operator*() noexcept
			{
				return m_ID;
			}

			const EventID* operator&() const noexcept
			{
				return &m_ID;
			}
			EventID* operator&() noexcept
			{
				return &m_ID;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			operator const EventID&() const noexcept
			{
				return m_ID;
			}
			operator EventID&() noexcept
			{
				return m_ID;
			}

			bool operator==(const EventTag& other) const noexcept
			{
				return m_ID == other.m_ID;
			}
			bool operator!=(const EventTag& other) const noexcept
			{
				return m_ID != other.m_ID;
			}

			EventTag& operator=(EventTag&& other) noexcept
			{
				m_ID = std::move(other.m_ID);
				return *this;
			}
			EventTag& operator=(const EventTag&) = default;
	};
}

namespace kxf::EventSystem
{
	EventID NewSimpleEventID() noexcept;
	EventID NewUniqueEventID() noexcept;
}

namespace std
{
	template<>
	struct hash<kxf::EventID>
	{
		size_t operator()(const kxf::EventID& id) const noexcept
		{
			return id.GetHash();
		}
	};
}
