#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/UniversallyUniqueID.h"
#include <type_traits>
#include <variant>

namespace kxf::EventSystem::Private
{
	template<class TFunc, class = std::enable_if_t<std::is_member_function_pointer_v<TFunc>>>
	UniversallyUniqueID StoreMemberFunction(TFunc func) noexcept
	{
		static_assert(sizeof(func) <= sizeof(NativeUUID));

		NativeUUID uuid;
		std::memcpy(&uuid, &func, sizeof(func));
		return uuid;
	}
}

namespace kxf
{
	class KX_API EventID final
	{
		private:
			std::variant<int64_t, UniversallyUniqueID, String> m_ID;

		public:
			EventID() noexcept = default;

			// Null pointer
			EventID(std::nullptr_t) noexcept
				:m_ID(static_cast<int64_t>(0))
			{
			}

			// Member function pointer
			template<class TFunc, class = std::enable_if_t<std::is_member_function_pointer_v<TFunc>>>
			EventID(TFunc func) noexcept
				:m_ID(EventSystem::Private::StoreMemberFunction(func))
			{
			}
			
			// Simple integer
			EventID(int64_t id) noexcept
				:m_ID(id)
			{
			}
			EventID(int id) noexcept
				:m_ID(static_cast<int64_t>(id))
			{
			}
			
			// UUID
			EventID(UniversallyUniqueID id) noexcept
				:m_ID(std::move(id))
			{
			}
			
			// String
			EventID(String id) noexcept
				:m_ID(std::move(id))
			{
			}
			EventID(const char* id) noexcept
				:m_ID(String(id))
			{
			}
			EventID(const wchar_t* id) noexcept
				:m_ID(String(id))
			{
			}
			
			// Wx event tag (integer)
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

			int64_t AsInt() const noexcept;
			UniversallyUniqueID AsUniqueID() const noexcept;
			const String& AsString() const noexcept;

			bool IsWxWidgetsID() const noexcept;

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
				return this == &other || m_ID == other.m_ID;
			}
			bool operator!=(const EventID& other) const noexcept
			{
				return this != &other && m_ID != other.m_ID;
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
				return m_ID.IsNull();
			}
			wxEventTypeTag<T> ToWxTag() const noexcept
			{
				return m_ID.AsInt();
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

			const EventID* operator->() const noexcept
			{
				return &m_ID;
			}
			EventID* operator->() noexcept
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
	KX_API EventID NewSimpleEventID() noexcept;
	KX_API EventID NewUniqueEventID() noexcept;
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
