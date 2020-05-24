#pragma once
#include "Kx/Network/Common.h"
#include "Kx/General/String.h"

namespace kxf
{
	class CURLSession;
	class CURLReplyBase;
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

			int SetOption(int option, const String& value, size_t* length = nullptr);
			int SetOption(int option, int32_t value) noexcept;
			int SetOption(int option, int64_t value) noexcept;
			int SetOption(int option, size_t value) noexcept;
			int SetOption(int option, bool value) noexcept;
			int SetOption(int option, const void* value) noexcept;

			template<class T, class = std::enable_if_t<std::is_function_v<std::remove_pointer_t<T>>>>
			int SetOption(int option, T value) noexcept
			{
				return SetOption(option, reinterpret_cast<const void*>(value));
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

	class CallbackData final
	{
		private:
			CURLSession& m_Session;
			CURLReplyBase& m_Reply;

		public:
			CallbackData(CURLSession& session, CURLReplyBase& reply)
				:m_Session(session), m_Reply(reply)
			{
			}

		public:
			CURLSession& GetSession()
			{
				return m_Session;
			}
			CURLReplyBase& GetReply()
			{
				return m_Reply;
			}
	};
}
