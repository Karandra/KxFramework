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
			SendEvent(NewEvent(KxEVT_WEBSOCKET_OPEN));
		});
		m_Client.set_fail_handler([this](TConnectionHandle handle)
		{
			SendEvent(NewEvent(KxEVT_WEBSOCKET_FAIL));
		});
		m_Client.set_close_handler([this](TConnectionHandle handle)
		{
			SendEvent(NewEvent(KxEVT_WEBSOCKET_CLOSE));
		});
		m_Client.set_message_handler([this](TConnectionHandle handle, TMessagePtr message)
		{
			switch (message->get_opcode())
			{
				case TFrameOpCode::text:
				{
					const std::string& payload = message->get_payload();

					auto event = NewEvent(KxEVT_WEBSOCKET_MESSAGE);
					event->SetString(wxString::FromUTF8(payload.data(), payload.size()));
					SendEvent(std::move(event));
					break;
				}
				case TFrameOpCode::binary:
				{
					const std::string& payload = message->get_payload();

					auto event = NewEvent(KxEVT_WEBSOCKET_MESSAGE);
					event->SetBinaryMessage(payload.data(), payload.size());
					SendEvent(std::move(event));
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
				wxLogDebug(wxS("KxWebSocket::RegisterHandlers: exception occured"));
			}
			return context;
		});
	}
	void SecureClient::AddRequestHeaders()
	{
		for (const auto& [key, value]: m_Headers)
		{
			try
			{
				m_ConnectionPtr->append_header(ToUTF8(key), ToUTF8(value));
			}
			catch (...)
			{
				wxLogDebug(wxS("KxWebSocket::AddRequestHeaders: exception occured"));
			}
		}
	}

	std::unique_ptr<KxWebSocketEvent> SecureClient::NewEvent(wxEventType eventType)
	{
		auto event = std::make_unique<KxWebSocketEvent>(eventType, KxID_NONE);
		event->SetURI(m_Address);

		return event;
	}
	void SecureClient::SendEvent(std::unique_ptr<KxWebSocketEvent> event)
	{
		QueueEvent(event.release());
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
	bool SecureClient::DoConnect(const KxURI& address)
	{
		if (address)
		{
			m_Address = address;
		}

		TErrorCode errorCode;
		m_ConnectionPtr = m_Client.get_connection(ToUTF8(m_Address.BuildURI()), errorCode);
		if (!errorCode)
		{
			AddRequestHeaders();

			KxWebSocketEvent event(KxEVT_WEBSOCKET_CONNECTING, KxID_NONE);
			event.SetURI(address);
			event.SetEventObject(this);
			ProcessEvent(event);

			m_Client.connect(m_ConnectionPtr);
			std::thread([this]()
			{
				try
				{
					m_Client.run();
				}
				catch (...)
				{
					wxLogDebug(wxS("KxWebSocket::SecureClient::<ASIOLoop>: exception occured"));
				}
			}).detach();
			return true;
		}
		return false;
	}

	SecureClient::SecureClient(const KxURI& address)
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
		if (m_ConnectionPtr)
		{
			try
			{
				m_ConnectionPtr->append_header(ToUTF8(key), ToUTF8(value));
			}
			catch (...)
			{
				wxLogDebug(wxS("KxWebSocket::AddHeader: exception occured"));
			}
		}
		else
		{
			m_Headers.insert_or_assign(key, value);
		}
	}
	void SecureClient::ReplaceHeader(const wxString& key, const wxString& value)
	{
		if (m_ConnectionPtr)
		{
			try
			{
				m_ConnectionPtr->replace_header(ToUTF8(key), ToUTF8(value));
			}
			catch (...)
			{
				wxLogDebug(wxS("KxWebSocket::ReplaceHeader: exception occured"));
			}
		}
		else
		{
			m_Headers.insert_or_assign(key, value);
		}
	}
	void SecureClient::RemoveHeader(const wxString& key)
	{
		if (m_ConnectionPtr)
		{
			try
			{
				m_ConnectionPtr->remove_header(ToUTF8(key));
			}
			catch (...)
			{
				wxLogDebug(wxS("KxWebSocket::RemoveHeader: exception occured"));
			}
		}
		else
		{
			m_Headers.erase(key);
		}
	}
	void SecureClient::ClearHeaders()
	{
		m_Headers.clear();
	}

	void SecureClient::SetUserAgent(const wxString& userAgent)
	{
		m_Client.set_user_agent(ToUTF8(userAgent));
	}
}
