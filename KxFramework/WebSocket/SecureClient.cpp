/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "SecureClient.h"
#include "KxFramework/KxWebSocket.h"

namespace KxWebSocket
{
	void SecureClient::RegisterHandlers()
	{
		m_Client.set_open_handler([this](TConnectionHandle handle)
		{
			m_ConnectionHandle = handle;
			QueueEvent(NewEvent(KxEVT_WEBSOCKET_OPEN));
		});
		m_Client.set_fail_handler([this](TConnectionHandle handle)
		{
			QueueEvent(NewEvent(KxEVT_WEBSOCKET_FAIL));
		});
		m_Client.set_close_handler([this](TConnectionHandle handle)
		{
			QueueEvent(NewEvent(KxEVT_WEBSOCKET_CLOSE));
		});
		m_Client.set_message_handler([this](TConnectionHandle handle, TMessagePtr message)
		{
			switch (message->get_opcode())
			{
				case TFrameOpCode::text:
				{
					const std::string& payload = message->get_payload();

					KxWebSocketEvent* event = NewEvent(KxEVT_WEBSOCKET_MESSAGE);
					event->SetString(wxString::FromUTF8(payload.data(), payload.size()));
					QueueEvent(event);
					break;
				}
				case TFrameOpCode::binary:
				{
					const std::string& payload = message->get_payload();

					KxWebSocketEvent* event = NewEvent(KxEVT_WEBSOCKET_MESSAGE);
					event->SetBinaryMessage(payload.data(), payload.size());
					QueueEvent(event);
					break;
				}
			};
		});
	
		m_Client.set_tls_init_handler([this](websocketpp::connection_hdl hdl)
		{
			TSSLContext context = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
			try
			{
				context->set_options(boost::asio::ssl::context::default_workarounds |
									 boost::asio::ssl::context::no_sslv2 |
									 boost::asio::ssl::context::no_sslv3 |
									 boost::asio::ssl::context::single_dh_use
				);
			}
			catch (...)
			{
			}
			return context;
		});
	}
	KxWebSocketEvent* SecureClient::NewEvent(wxEventType eventType)
	{
		return new KxWebSocketEvent(eventType, KxID_NONE);
	}
	void SecureClient::QueueEvent(wxEvent* event)
	{
		ProcessEvent(*event);
		delete event;
	}

	std::string SecureClient::ToUTF8(const wxString& string) const
	{
		auto utf8 = string.ToUTF8();
		return std::string(utf8.data(), utf8.length());
	}
	wxString SecureClient::FromUTF8(const std::string& string) const
	{
		return wxString::FromUTF8(string.data(), string.size());
	}

	bool SecureClient::DoSendData(const void* data, size_t length)
	{
		TErrorCode errorCode = m_ConnectionPtr->send(data, length, TFrameOpCode::binary);
		return !errorCode;
	}
	bool SecureClient::DoSendData(const wxString& stringData)
	{
		std::string utf8 = ToUTF8(stringData);
		TErrorCode errorCode = m_ConnectionPtr->send(utf8, TFrameOpCode::text);
		return !errorCode;
	}

	void SecureClient::DoClose(CloseCode code, const wxString& status, int& errorCode)
	{
		m_Client.close(m_ConnectionHandle, static_cast<TCloseStatus>(code), ToUTF8(status));
	}
	bool SecureClient::DoConnect(const wxString& address)
	{
		TErrorCode errorCode;
		m_ConnectionPtr = m_Client.get_connection(ToUTF8(!address.IsEmpty() ? address : m_Address), errorCode);
		if (!errorCode)
		{
			m_Client.connect(m_ConnectionPtr);

			std::thread asioThread([this]()
			{
				try
				{
					m_Client.run();
				}
				catch (...)
				{
					wxLogDebug(wxS("KxWebSocket::SecureClient::<ASIOLoop>: exception occured"));
				}
			});
			asioThread.detach();
			return true;
		}
		return false;
	}

	SecureClient::SecureClient(const wxString& address)
		:m_Address(address)
	{
		m_Client.clear_access_channels(websocketpp::log::alevel::frame_header);
		m_Client.clear_access_channels(websocketpp::log::alevel::frame_payload);
		
		m_Client.init_asio();
		m_Client.start_perpetual();
		RegisterHandlers();
	}
	SecureClient::~SecureClient()
	{
		m_Client.stop_perpetual();
		m_Client.stop();
	}

	bool SecureClient::IsOK() const
	{
		return m_ConnectionPtr != nullptr;
	}
	bool SecureClient::IsConnected() const
	{
		return m_ConnectionPtr && m_ConnectionPtr->get_state() == TConnectionState::open && !m_ConnectionHandle.expired();
	}
	bool SecureClient::IsSecure() const
	{
		return m_Client.is_secure();
	}
	
	CloseCode SecureClient::GetCloseCode() const
	{
		if (m_ConnectionPtr)
		{
			return static_cast<CloseCode>(m_ConnectionPtr->get_remote_close_code());
		}
		return CloseCode::NoStatus;
	}
	wxString SecureClient::GetCloseReason() const
	{
		if (m_ConnectionPtr)
		{
			return FromUTF8(m_ConnectionPtr->get_remote_close_reason());
		}
		return {};
	}

	void SecureClient::AddHeader(const wxString& key, const wxString& value)
	{
		try
		{
			m_ConnectionPtr->append_header(ToUTF8(key), ToUTF8(value));
		}
		catch (...)
		{
		}
	}
	void SecureClient::ReplaceHeader(const wxString& key, const wxString& value)
	{
		m_ConnectionPtr->replace_header(ToUTF8(key), ToUTF8(value));
	}

	void SecureClient::SetUserAgent(const wxString& userAgent)
	{
		m_Client.set_user_agent(ToUTF8(userAgent));
	}
}
