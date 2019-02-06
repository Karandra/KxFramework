/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "IClient.h"

#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/client.hpp"

namespace KxWebSocket
{
	class KX_API SecureClient: public IClient
	{
		private:
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
			wxString m_Address;

		private:
			void RegisterHandlers();
			KxWebSocketEvent* NewEvent(wxEventType eventType);
			void QueueEvent(wxEvent* event)
			{
				ProcessEvent(*event);
				delete event;
			}

			std::string ToUTF8(const wxString& string) const;
			wxString FromUTF8(const std::string& string) const;

		protected:
			bool DoSendData(const void* data, size_t length) override;
			bool DoSendData(const wxString& stringData) override;
			void DoClose(CloseCode code, const wxString& status, int& errorCode) override;
			bool DoConnect(const wxString& address) override;

		public:
			SecureClient(const wxString& address);
			~SecureClient();

		public:
			bool IsOK() const override;
			bool IsConnected() const override;
			bool IsSecure() const override;

			CloseCode GetCloseCode() const override;
			wxString GetCloseReason() const override;
	};
}
