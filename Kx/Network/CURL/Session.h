#pragma once
#include "Kx/Network/Common.h"
#include "Private/CURL.h"
#include "Reply.h"
#include "Event.h"
#include "Kx/Network/URI.h"

namespace KxFramework
{
	class KX_API CURLSession: public wxEvtHandler
	{
		friend class CURLEvent;

		private:
			CURL::Private::SessionHandle m_Handle;
			bool m_IsPaused = false;
			bool m_IsStopped = false;
			
			std::vector<std::string> m_SessionHeaders;
			void* m_HeadersSList = nullptr;

			URI m_URI;
			String m_PostData;
			String m_UserAgent;

		private:
			static size_t OnWriteResponse(char* data, size_t size, size_t count, void* userData);
			static size_t OnWriteHeader(char* data, size_t size, size_t count, void* userData);

		private:
			void SetHeaders();
			void DoSendRequest(CURLReplyBase& reply);

		public:
			CURLSession(const URI& url = {});
			CURLSession(const CURLSession&) = delete;
			CURLSession(CURLSession&& other) noexcept
			{
				*this = std::move(other);
			}
			~CURLSession() noexcept
			{
				Close();
			}

		public:
			void Close() noexcept;
			CURL::Private::SessionHandle GetHandle() const noexcept
			{
				return m_Handle;
			}

			CURLStringReply Send()
			{
				CURLStringReply reply;
				DoSendRequest(reply);
				return reply;
			}
			CURLBinaryReply Download()
			{
				CURLBinaryReply reply;
				DoSendRequest(reply);
				return reply;
			}
			void Download(CURLStreamReply& reply);
			size_t EnumReplyCookies(std::function<bool(String)> func) const;

			bool IsPaused() const noexcept
			{
				return m_IsPaused;
			}
			bool Pause() noexcept;
			bool Resume() noexcept;

			bool IsStopped() const noexcept
			{
				return m_IsStopped;
			}
			void Stop() noexcept;

			void SetURI(const URI& uri);
			void SetPostData(const String& data);

			void ClearHeaders() noexcept
			{
				m_SessionHeaders.clear();
			}
			void AddHeader(const String& name, const String& value);
			void AddHeader(const String& value);
			void SetUserAgent(const String& userAgent)
			{
				m_UserAgent = userAgent;
			}

			void SetTimeout(const TimeSpan& timeout) noexcept;
			void SetConnectionTimeout(const TimeSpan& timeout) noexcept;

		public:
			CURLSession& operator=(const CURLSession&) = delete;
			CURLSession& operator=(CURLSession&& other) noexcept;

			explicit operator bool() const noexcept
			{
				return !m_Handle.IsNull();
			}
			bool operator!() const noexcept
			{
				return m_Handle.IsNull();
			}
	};
}
