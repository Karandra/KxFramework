#pragma once
#include "Common.h"
#include "IWebRequest.h"
#include "IWebResponse.h"
#include "WebRequestHeader.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class WebRequestEvent: public BasicEvent
	{
		public:
			KxEVENT_MEMBER(WebRequestEvent, StateChanged);
			KxEVENT_MEMBER(WebRequestEvent, HeaderReceived);

			KxEVENT_MEMBER(WebRequestEvent, DataSent);
			KxEVENT_MEMBER(WebRequestEvent, DataReceived);

		private:
			IWebRequest* m_Request = nullptr;
			IWebResponse* m_Response = nullptr;
			WebRequestState m_State = WebRequestState::None;

			HTTPStatus m_Status;
			String m_ErrorMessage;

			WebRequestHeader m_Header;
			const void* m_Buffer = nullptr;
			size_t m_BufferSize = 0;

		public:
			WebRequestEvent(IWebRequest& request, WebRequestState state)
				:m_Request(&request), m_State(state)
			{
			}
			WebRequestEvent(IWebRequest& request, WebRequestState state, WebRequestHeader header)
				:m_Request(&request), m_State(state), m_Header(std::move(header))
			{
			}
			WebRequestEvent(IWebRequest& request, WebRequestState state, HTTPStatus status, String errorMessage = {})
				:m_Request(&request), m_State(state), m_Status(std::move(status)), m_ErrorMessage(std::move(errorMessage))
			{
			}
			WebRequestEvent(IWebRequest& request, WebRequestState state, const void* buffer, size_t bufferSize)
				:m_Request(&request), m_State(state), m_Buffer(buffer), m_BufferSize(bufferSize)
			{
			}
			WebRequestEvent(IWebRequest& request, IWebResponse& response)
				:m_Request(&request), m_Response(&response), m_State(WebRequestState::Completed)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WebRequestEvent>(std::move(*this));
			}

			// WebRequestEvent
			IWebRequest& GetRequest() const
			{
				return *m_Request;
			}
			IWebResponse& GetResponse() const
			{
				return m_Response ? *m_Response : NullWebResponse::Get();
			}
			WebRequestState GetState() const
			{
				return m_State;
			}

			const WebRequestHeader& GetHeader() const&
			{
				return m_Header;
			}
			WebRequestHeader GetHeader() &&
			{
				return std::move(m_Header);
			}

			const String& GetErrorMessage() const&
			{
				return m_ErrorMessage;
			}
			String GetErrorMessage() &&
			{
				return std::move(m_ErrorMessage);
			}
			HTTPStatus GetStatus()
			{
				return m_Status;
			}

			const void* GetBuffer() const
			{
				return m_Buffer;
			}
			size_t GetBufferSize() const
			{
				return m_BufferSize;
			}
	};
}
