#pragma once
#include "kxf/Network/Common.h"
#include "kxf/Core/String.h"

namespace kxf
{
	class CURLWebRequest;
}

namespace kxf::CURL::Private
{
	enum class HandleType
	{
		Easy,
		Multi
	};

	class RequestHandle final
	{
		private:
			void* m_Handle = nullptr;
			HandleType m_Type = HandleType::Easy;

		public:
			RequestHandle() noexcept = default;
			RequestHandle(HandleType type) noexcept;
			RequestHandle(const RequestHandle&) = delete;
			~RequestHandle();

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			HandleType GetType() const noexcept
			{
				return m_Type;
			}

		public:
			// Common
			bool SetOption(int option, const std::string& utf8);
			bool SetOption(int option, const String& value, size_t* length = nullptr);
			bool SetOption(int option, bool value);
			bool SetOption(int option, int32_t value);
			bool SetOption(int option, uint32_t value);
			bool SetOption(int option, int64_t value);
			bool SetOption(int option, uint64_t value);
			bool SetOption(int option, const void* value);

			template<class T> requires(std::is_function_v<std::remove_pointer_t<T>> && !std::is_member_function_pointer_v<T>)
			bool SetOption(int option, T value)
			{
				return SetOption(option, reinterpret_cast<const void*>(value));
			}

			template<class T> requires(std::is_member_function_pointer_v<T>)
			bool SetOption(int option, T value)
			{
				static_assert(sizeof(T*) == 0, "member function pointers aren't allowed here");
				return false;
			}

			template<class T>
			bool SetOption(int option, const FlagSet<T>& value)
			{
				return SetOption(option, value.ToInt());
			}

			bool SetOption(int option, long value)
			{
				return SetOption(option, static_cast<int32_t>(value));
			}
			bool SetOption(int option, unsigned long value)
			{
				return SetOption(option, static_cast<uint32_t>(value));
			}

		public:
			// Easy API
			std::string EscapeString(std::string_view source) const;
			std::string UnescapeString(std::string_view source) const;

			bool Pause() noexcept;
			bool Resume() noexcept;

			std::optional<std::string_view> GetOptionUTF8(int option) const;
			std::optional<String> GetOptionString(int option) const;
			std::optional<bool> GetOptionBool(int option) const;
			std::optional<int32_t> GetOptionInt32(int option) const;
			std::optional<uint32_t> GetOptionUInt32(int option) const;
			std::optional<int64_t> GetOptionInt64(int option) const;
			std::optional<uint64_t> GetOptionUInt64(int option) const;
			std::optional<void*> GetOptionPtr(int option) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			void* operator*() const noexcept
			{
				return m_Handle;
			}

			RequestHandle& operator=(const RequestHandle&) = delete;
	};
}

namespace kxf::CURL::Private
{
	bool Initialize() noexcept;

	std::string_view EasyErrorCodeToString(int easyErrorCode) noexcept;
}
