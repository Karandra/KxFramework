#include "KxFramework/KxWebSocketsImpl.h"

// Setup versions
#pragma warning(disable: 4005) // macro redefinition
#pragma warning(disable: 4267) // conversion from 'x' to 'y', possible loss of data
#include <SDKDDKVer.h>
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define _WIN32_WINDOWS _WIN32_WINNT_WIN7

// Include boost
#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/client.hpp"
#include "websocketpp/version.hpp"

//////////////////////////////////////////////////////////////////////////
using WebSocketClient = websocketpp::client<websocketpp::config::asio_client>;
using WebSocketClientTLS = websocketpp::client<websocketpp::config::asio_tls_client>;

using WebSocketClientConnection = typename WebSocketClient::connection_ptr;
using WebSocketClientConnectionPtr = typename WebSocketClientConnection::element_type;

using WebSocketClientConnectionTLS = typename WebSocketClientTLS::connection_ptr;
using WebSocketClientConnectionTLSPtr = typename WebSocketClientConnectionTLS::element_type;

using WebSocketMessage = websocketpp::config::asio_client::message_type::ptr;
using WebSocketMessageTLS = websocketpp::config::asio_tls_client::message_type::ptr;

using WebSocketOpcode = websocketpp::frame::opcode::value;
using WebSocketState = websocketpp::session::state::value;

using WebSocketContextTLS = std::shared_ptr<boost::asio::ssl::context>;

namespace
{
	inline WebSocketClient* GetAsClient(void* client)
	{
		return reinterpret_cast<WebSocketClient*>(client);
	}
	inline WebSocketClientTLS* GetAsClientTLS(void* client)
	{
		return reinterpret_cast<WebSocketClientTLS*>(client);
	}

	inline WebSocketClientConnectionPtr* GetAsConnection(void* connection)
	{
		return reinterpret_cast<WebSocketClientConnectionPtr*>(connection);
	}
	inline WebSocketClientConnectionTLSPtr* GetAsConnectionTLS(void* connection)
	{
		return reinterpret_cast<WebSocketClientConnectionTLSPtr*>(connection);
	}

	inline WebSocketMessage* GetAsMessage(void* message)
	{
		return reinterpret_cast<WebSocketMessage*>(message);
	}
	inline WebSocketMessageTLS* GetAsMessageTLS(void* message)
	{
		return reinterpret_cast<WebSocketMessageTLS*>(message);
	}
}

//////////////////////////////////////////////////////////////////////////
std::string KxWebSocketClientImpl::GetVersion()
{
	std::string version(16, '\000');
	int length = sprintf_s(version.data(), version.size(), "%d.%d.%d", websocketpp::major_version, websocketpp::minor_version, websocketpp::patch_version);
	version.resize(length);

	return version;
}

void KxWebSocketClientImpl::OnMessage(const std::string& message, int opCode)
{
	switch (opCode)
	{
		case WebSocketOpcode::text:
		{
			m_MessageHandler->OnTextMessage(message.c_str(), message.length());
			break;
		}

		default:
		{
			m_MessageHandler->OnBinaryMessage(message.c_str(), message.length());
		}
	};
}
void KxWebSocketClientImpl::OnLogMessage(const char* message, int logLevel)
{
}
void KxWebSocketClientImpl::Destroy()
{
	Close(KxWS_CLOSE_NORMAL);
	if (m_IsSecure)
	{
		delete GetAsClientTLS(m_Client);
	}
	else
	{
		delete GetAsClient(m_Client);
	}
	m_Client = nullptr;
}
bool KxWebSocketClientImpl::DoClose(int code, const std::string& reason, int& errorCode)
{
	if (m_Connection)
	{
		std::error_code error;
		if (m_IsSecure)
		{
			GetAsConnectionTLS(m_Connection)->close(code, reason, error);
		}
		else
		{
			GetAsConnection(m_Connection)->close(code, reason, error);
		}
		m_Connection = nullptr;
		errorCode = error.value();

		return error.value() == 0;
	}
	return false;
}
bool KxWebSocketClientImpl::IsWSS(const std::string& url) const
{
	size_t pos = url.find(':');
	if (pos != std::string::npos)
	{
		// For 'wss://example.com:80' pos will be '3'.
		return pos == 3;
	}
	return false;
}

KxWebSocketClientImpl::KxWebSocketClientImpl(MessageHandler* handler, const std::string& url)
	:m_MessageHandler(handler), m_URL(url), m_IsSecure(IsWSS(url))
{
	if (m_IsSecure)
	{
		m_Client = new WebSocketClientTLS();

		InitClient<WebSocketClientTLS>();
		RigisterHandlers<WebSocketClientTLS, WebSocketMessageTLS>();

		// Can generalize this
		GetAsClientTLS(m_Client)->set_tls_init_handler([this](websocketpp::connection_hdl hdl)
		{
			WebSocketContextTLS context = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
			try
			{
				context->set_options(boost::asio::ssl::context::default_workarounds |
									 boost::asio::ssl::context::no_sslv2 |
									 boost::asio::ssl::context::no_sslv3 |
									 boost::asio::ssl::context::single_dh_use
				);
			}
			catch (std::exception& exception)
			{
				std::string error("Error in context pointer: ");
				error += exception.what();

				m_MessageHandler->OnLogFatal(error.c_str());
			}
			return context;
		});

		CreateConnection<WebSocketClientTLS, WebSocketClientConnectionTLS>();
	}
	else
	{
		m_Client = new WebSocketClient();

		InitClient<WebSocketClient>();
		RigisterHandlers<WebSocketClient, WebSocketMessage>();
		CreateConnection<WebSocketClient, WebSocketClientConnection>();
	}
	
	#if 0
	// Register our handlers
	client->set_open_handler([this](websocketpp::connection_hdl hdl)
	{
		m_MessageHandler->OnOpen();
	});
	client->set_close_handler([this](websocketpp::connection_hdl hdl)
	{
		m_MessageHandler->OnClose();
	});
	client->set_fail_handler([this](websocketpp::connection_hdl hdl)
	{
		m_MessageHandler->OnFail();
	});
	client->set_message_handler([this](websocketpp::connection_hdl hdl, WebSocketMessage message)
	{
		OnMessage(message->get_payload(), message->get_opcode());
	});
	#if 0
	client->set_tls_init_handler([this](websocketpp::connection_hdl hdl)
	{
		WebSocketSecureContext context = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
		try
		{
			context->set_options(boost::asio::ssl::context::default_workarounds|
								 boost::asio::ssl::context::no_sslv2|
								 boost::asio::ssl::context::no_sslv3|
								 boost::asio::ssl::context::single_dh_use
			);
		}
		catch (std::exception& exception)
		{
			std::string error("Error in context pointer: ");
			error += exception.what();

			m_MessageHandler->OnLogFatal(error.c_str());
		}
		return context;
	});
	#endif

	std::error_code connectionErrorCode;
	WebSocketClientConnection connection = client->get_connection(url, connectionErrorCode);
	if (connectionErrorCode)
	{
		std::string error("Could not create connection because: ");
		error += connectionErrorCode.message();

		m_MessageHandler->OnLogFatal(error.c_str());
		Destroy();
	}
	else
	{
		// Note that connect here only requests a connection. No network messages are
		// exchanged until the event loop starts running in the next line.
		client->connect(connection);
		m_Connection = connection.get();

		// Start the ASIO io_service run loop
		// this will cause a single connection to be made to the server. c.run()
		// will exit when this connection is closed.
		std::thread asioThread(&WebSocketClient::run, client);
		asioThread.detach();
	}
	#endif
}
KxWebSocketClientImpl::~KxWebSocketClientImpl()
{
	Destroy();
}

bool KxWebSocketClientImpl::IsOK() const
{
	if (m_Client)
	{
		if (m_IsSecure)
		{
			return GetAsConnectionTLS(m_Connection)->get_state() == WebSocketState::open;
		}
		else
		{
			return GetAsConnection(m_Connection)->get_state() == WebSocketState::open;
		}
	}
	return false;
}
int KxWebSocketClientImpl::GetCloseCode() const
{
	if (m_IsSecure)
	{
		return GetAsConnectionTLS(m_Connection)->get_remote_close_code();
	}
	else
	{
		return GetAsConnection(m_Connection)->get_remote_close_code();
	}
}

bool KxWebSocketClientImpl::SendBinary(const void* data, size_t length)
{
	std::error_code errorCode;
	if (m_IsSecure)
	{
		errorCode = GetAsConnectionTLS(m_Connection)->send(data, length);
	}
	else
	{
		errorCode = GetAsConnection(m_Connection)->send(data, length);
	}

	if (errorCode)
	{
		m_MessageHandler->OnLogError(errorCode.message());
		return false;
	}
	return true;
}
bool KxWebSocketClientImpl::SendText(const char* stringUTF8, size_t length)
{
	std::error_code errorCode;
	if (m_IsSecure)
	{
		errorCode = GetAsConnectionTLS(m_Connection)->send(std::string(stringUTF8, length));
	}
	else
	{
		errorCode = GetAsConnection(m_Connection)->send(std::string(stringUTF8, length));
	}

	if (errorCode)
	{
		m_MessageHandler->OnLogError(errorCode.message());
		return false;
	}
	return true;
}
