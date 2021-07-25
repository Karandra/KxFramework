#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/Utility/Memory.h"
#include <type_traits>
#include <variant>

namespace kxf
{
	class KX_API EventID final
	{
		friend struct std::hash<EventID>;
		friend struct BinarySerializer<EventID>;

		private:
			std::variant<int64_t, UniversallyUniqueID, String> m_ID;

		private:
			size_t GetHash() const noexcept;
			uint64_t Serialize(IOutputStream& stream) const;
			uint64_t Deserialize(IInputStream& stream);

		public:
			EventID() noexcept = default;
			EventID(bool) = delete;

			// Null pointer
			EventID(std::nullptr_t) noexcept
				:m_ID(static_cast<int64_t>(0))
			{
			}

			// Member function pointer
			template<class TFunc> requires(std::is_member_function_pointer_v<TFunc>)
			EventID(TFunc func) noexcept
				:m_ID(Utility::StoreMemberFunction(func).ToUniversallyUniqueID())
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

			std::strong_ordering operator<=>(const EventID&) const noexcept = default;
			bool operator==(const EventID&) const noexcept = default;

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

			auto operator<=>(const EventTag&) const noexcept = default;
			bool operator==(const EventTag&) const noexcept = default;

			auto operator<=>(const EventID& other) const noexcept
			{
				return m_ID <=> other;
			}
			bool operator==(const EventID& other) const noexcept
			{
				return m_ID == other;
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

namespace kxf
{
	template<>
	struct BinarySerializer<EventID> final
	{
		uint64_t Serialize(IOutputStream& stream, const EventID& value) const
		{
			return value.Serialize(stream);
		}
		uint64_t Deserialize(IInputStream& stream, EventID& value) const
		{
			return value.Deserialize(stream);
		}
	};
}

namespace std
{
	template<>
	struct hash<kxf::EventID> final
	{
		size_t operator()(const kxf::EventID& id) const noexcept
		{
			return id.GetHash();
		}
	};
}
