#pragma once
#include "../Common.h"
#include "Kx/FileSystem/FileOperationEvent.h"

namespace kxf
{
	class KX_API CURLSession;
	class KX_API ICURLReply;
}

namespace kxf
{
	class KX_API CURLEvent: public FileOperationEvent
	{
		friend class CURLSession;

		public:
			KxEVENT_MEMBER(CURLEvent, Download);
			KxEVENT_MEMBER(CURLEvent, ResponseHeader);

		private:
			CURLSession* m_Session = nullptr;
			ICURLReply* m_Reply = nullptr;

			const char* m_ResponseData = nullptr;
			size_t m_ResponseLength = 0;

		private:
			void SetResponseData(const char* data, size_t length) noexcept
			{
				m_ResponseData = data;
				m_ResponseLength = length;
			}

		public:
			CURLEvent() = default;
			CURLEvent(EventID eventType, CURLSession* session, ICURLReply* reply)
				:FileOperationEvent(eventType), m_Session(session), m_Reply(reply)
			{
			}

		public:
			CURLEvent* Clone() const override
			{
				return new CURLEvent(*this);
			}
			bool IsNull() const noexcept
			{
				return m_Session == nullptr || m_Reply == nullptr;
			}

			CURLSession& GetSession() const noexcept
			{
				return *m_Session;
			}
			ICURLReply& GetReply() const noexcept
			{
				return *m_Reply;
			}

			String GetHeaderName() const;
			String GetHeaderValue() const;
			String GetHeaderLine() const;

		public:
			wxDECLARE_DYNAMIC_CLASS(CURLEvent);
	};
}
