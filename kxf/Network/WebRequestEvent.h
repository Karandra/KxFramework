#pragma once
#include "Common.h"
#include "IWebRequest.h"
#include "IWebResponse.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class WebRequestEvent: public BasicEvent
	{
		public:
			KxEVENT_MEMBER(WebRequestEvent, StateChanged);
			KxEVENT_MEMBER(WebRequestEvent, DataDownloaded);

		private:
			IWebRequest* m_Request = nullptr;
			IWebResponse* m_Response = nullptr;
			WebRequestState m_State = WebRequestState::None;

		public:
			WebRequestEvent(IWebRequest& request, WebRequestState state)
				:m_Request(&request), m_State(state)
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
	};
}
