#pragma once
#include "kxf/Network/Common.h"
#include "kxf/General/String.h"

namespace kxf
{
	class CURLRequest;
}

namespace kxf::CURL::Private
{
	enum class SessionHandleType
	{
		Easy,
		Multi
	};

	class SessionHandle final
	{
		private:
			void* m_Handle = nullptr;
			SessionHandleType m_Type = SessionHandleType::Easy;

		public:
			SessionHandle() noexcept = default;
			SessionHandle(void* handle, SessionHandleType type) noexcept
				:m_Handle(handle), m_Type(type)
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			void* GetNativeHandle() const noexcept
			{
				return m_Handle;
			}
			SessionHandleType GetHandleType() const noexcept
			{
				return m_Type;
			}

			bool SetOption(int option, const std::string& utf8);
			bool SetOption(int option, const String& value, size_t* length = nullptr);
			bool SetOption(int option, int32_t value) noexcept;
			bool SetOption(int option, uint32_t value) noexcept;
			bool SetOption(int option, int64_t value) noexcept;
			bool SetOption(int option, uint64_t value) noexcept;
			bool SetOption(int option, bool value) noexcept;
			bool SetOption(int option, const void* value) noexcept;

			template<class T, std::enable_if_t<std::is_function_v<std::remove_pointer_t<T>> && !std::is_member_function_pointer_v<T>, int> = 0>
			bool SetOption(int option, T value) noexcept
			{
				return SetOption(option, reinterpret_cast<const void*>(value));
			}

			template<class T, std::enable_if_t<std::is_member_function_pointer_v<T>, int> = 0>
			bool SetOption(int option, T value) noexcept
			{
				static_assert(false, "member function pointers aren't allowed here");
				return false;
			}

			template<class T>
			bool SetOption(int option, const FlagSet<T>& value) noexcept
			{
				return SetOption(option, value.ToInt());
			}

			bool SetOption(int option, long value) noexcept
			{
				return SetOption(option, static_cast<int32_t>(value));
			}
			bool SetOption(int option, unsigned long value) noexcept
			{
				return SetOption(option, static_cast<uint32_t>(value));
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
	};
}

namespace kxf::CURL::Private
{
	std::string Escape(const SessionHandle& handle, std::string_view source);
	std::string Unescape(const SessionHandle& handle, std::string_view source);
}
