#pragma once
#include "../Common.h"
#include "Kx/Threading/JoinableThread.h"
#include "IClient.h"

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace kxf::WebSocket
{
	class KX_API SecureClient: public IWebSocketClient
	{
		public:
			using TClient = websocketpp::client<websocketpp::config::asio_tls_client>;
			using TConnectionHandle = websocketpp::connection_hdl;
			using TConnectionPtr = typename TClient::connection_ptr;

			using TMessagePtr = websocketpp::config::asio_tls_client::message_type::ptr;
			using TConnectionState = websocketpp::session::state::value;
			using TFrameOpCode = websocketpp::frame::opcode::value;
			using TCloseStatus = websocketpp::close::status::value;
			using TErrorCode = websocketpp::lib::error_code;
			using TSSLContext = std::shared_ptr<boost::asio::ssl::context>;

		private:
			TClient m_Client;
			TConnectionPtr m_ConnectionPtr;
			TConnectionHandle m_ConnectionHandle;

			JoinableThread m_Thread;
			std::unordered_map<String, String> m_SessionHeaders;
			URI m_Address;

		private:
			void RegisterHandlers();
			void AddRequestHeaders();

			std::unique_ptr<WebSocketEvent> NewEvent(EventID eventType)
			{
				auto event = std::make_unique<WebSocketEvent>(eventType, ToInt(StdID::None));
				event->SetURI(m_Address);

				return event;
			}
			void SendEvent(std::unique_ptr<WebSocketEvent> event)
			{
				QueueEvent(event.release());
			}

		protected:
			bool DoSendData(const void* data, size_t length) override;
			bool DoSendData(const String& stringData) override;
			void DoClose(CloseCode code, const String& status, int& errorCode) override;
			bool DoConnect(const URI& address) override;

		public:
			SecureClient(const URI& address);
			~SecureClient();

		public:
			bool IsNull() const
			{
				return m_ConnectionPtr == nullptr;
			}
			bool IsConnected() const override
			{
				return m_ConnectionPtr && m_ConnectionPtr->get_state() == TConnectionState::open && !m_ConnectionHandle.expired();
			}
			bool IsSecure() const override
			{
				return m_Client.is_secure();
			}

			CloseCode GetCloseCode() const override
			{
				if (m_ConnectionPtr)
				{
					return static_cast<CloseCode>(m_ConnectionPtr->get_remote_close_code());
				}
				return CloseCode::NoStatus;
			}
			String GetCloseReason() const override
			{
				if (m_ConnectionPtr)
				{
					return String::FromUTF8(m_ConnectionPtr->get_remote_close_reason());
				}
				return {};
			}

			void AddHeader(const String& key, const String& value) override;
			void ReplaceHeader(const String& key, const String& value) override;
			void RemoveHeader(const String& key) override;
			void ClearHeaders() override;
			void SetUserAgent(const String& userAgent) override;
	};
}
