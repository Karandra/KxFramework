#include "KxfPCH.h"
#include "SecureClient.h"

namespace kxf::WebSocket
{
	void SecureClient::RegisterHandlers()
	{
		m_Thread.Bind(ThreadEvent::EvtExecute, [this](ThreadEvent& event)
		{
			if (m_Thread.TestDestroy())
			{
				return;
			}

			try
			{
				m_Client.run();
			}
			catch (...)
			{
				wxLogDebug(wxS("WebSocket::SecureClient::<ASIOLoop>: exception occurred"));
			}
		});

		m_Client.set_open_handler([this](TConnectionHandle handle)
		{
			m_ConnectionHandle = handle;
			SendEvent(NewEvent(WebSocketEvent::EvtOpen));
		});
		m_Client.set_fail_handler([this](TConnectionHandle handle)
		{
			SendEvent(NewEvent(WebSocketEvent::EvtFail));
		});
		m_Client.set_close_handler([this](TConnectionHandle handle)
		{
			SendEvent(NewEvent(WebSocketEvent::EvtClose));
		});
		m_Client.set_message_handler([this](TConnectionHandle handle, TMessagePtr message)
		{
			switch (message->get_opcode())
			{
				case TFrameOpCode::text:
				{
					const std::string& payload = message->get_payload();

					auto event = NewEvent(WebSocketEvent::EvtMessage);
					event->SetTextMessage(String::FromUTF8(payload.data(), payload.size()));
					SendEvent(std::move(event));
					break;
				}
				case TFrameOpCode::binary:
				{
					const std::string& payload = message->get_payload();

					auto event = NewEvent(WebSocketEvent::EvtMessage);
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
		for (const auto& [key, value]: m_SessionHeaders)
		{
			try
			{
				m_ConnectionPtr->append_header(key.ToUTF8(), value.ToUTF8());
			}
			catch (...)
			{
				wxLogDebug(wxS("SecureClient::AddRequestHeaders: exception occurred"));
			}
		}
	}

	bool SecureClient::DoSendData(const void* data, size_t length)
	{
		TErrorCode errorCode = m_ConnectionPtr->send(data, length, TFrameOpCode::binary);
		return !errorCode;
	}
	bool SecureClient::DoSendData(const String& stringData)
	{
		TErrorCode errorCode = m_ConnectionPtr->send(stringData.ToUTF8(), TFrameOpCode::text);
		return !errorCode;
	}

	void SecureClient::DoClose(CloseCode code, const String& status, int& errorCode)
	{
		m_Client.close(m_ConnectionHandle, static_cast<TCloseStatus>(code), status.ToUTF8());
	}
	bool SecureClient::DoConnect(const URI& address)
	{
		if (address)
		{
			m_Address = address;
		}

		TErrorCode errorCode;
		m_ConnectionPtr = m_Client.get_connection(m_Address.BuildURI().ToUTF8(), errorCode);
		if (!errorCode)
		{
			AddRequestHeaders();

			WebSocketEvent event(WebSocketEvent::EvtConnecting, ToInt(StdID::None));
			event.SetURI(address);
			event.SetEventObject(this);
			ProcessEvent(event);

			m_Client.connect(m_ConnectionPtr);
			return m_Thread.Run() == wxTHREAD_NO_ERROR;
		}
		return false;
	}

	SecureClient::SecureClient(const URI& address)
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

	void SecureClient::AddHeader(const String& key, const String& value)
	{
		if (m_ConnectionPtr)
		{
			try
			{
				m_ConnectionPtr->append_header(key.ToUTF8(), value.ToUTF8());
			}
			catch (...)
			{
				wxLogDebug(wxS("KxWebSocket::AddHeader: exception occurred"));
			}
		}
		else
		{
			m_SessionHeaders.insert_or_assign(key, value);
		}
	}
	void SecureClient::ReplaceHeader(const String& key, const String& value)
	{
		if (m_ConnectionPtr)
		{
			try
			{
				m_ConnectionPtr->replace_header(key.ToUTF8(), value.ToUTF8());
			}
			catch (...)
			{
				wxLogDebug(wxS("KxWebSocket::ReplaceHeader: exception occurred"));
			}
		}
		else
		{
			m_SessionHeaders.insert_or_assign(key, value);
		}
	}
	void SecureClient::RemoveHeader(const String& key)
	{
		if (m_ConnectionPtr)
		{
			try
			{
				m_ConnectionPtr->remove_header(key.ToUTF8());
			}
			catch (...)
			{
				wxLogDebug(wxS("KxWebSocket::RemoveHeader: exception occured"));
			}
		}
		else
		{
			m_SessionHeaders.erase(key);
		}
	}
	void SecureClient::ClearHeaders()
	{
		m_SessionHeaders.clear();
	}

	void SecureClient::SetUserAgent(const String& userAgent)
	{
		m_Client.set_user_agent(userAgent.ToUTF8());
	}
}
