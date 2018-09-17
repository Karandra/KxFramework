#pragma once
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 1
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING 1

#include "KxFramework/KxWebSocketsImplHandler.h"
#include "KxFramework/KxWebSocketsConstants.h"
#include <string>
#include <thread>

class KxWebSocketClientImpl
{
	public:
		using HandlerFunc = void(*)(void* userData, const unsigned char* data, int dataLength);
		using MessageHandler = KxWebSocketsClientImplMessageHandler;

	public:
		static std::string GetVersion();

	private:
		MessageHandler* m_MessageHandler = nullptr;
		const std::string m_URL;
		const bool m_IsSecure = false;
		
		void* m_Client = nullptr;
		void* m_Connection = nullptr;

	private:
		template<class ClientT> ClientT* CastClient()
		{
			return reinterpret_cast<ClientT*>(m_Client);
		}

		template<class ClientT> void InitClient()
		{
			ClientT* client = CastClient<ClientT>();

			client->set_access_channels(websocketpp::log::alevel::all);
			client->set_error_channels(websocketpp::log::elevel::all);

			// Initialize ASIO
			client->init_asio();
		}
		template<class ClientT, class MessageT> void RigisterHandlers()
		{
			ClientT* client = CastClient<ClientT>();

			// Register our handlers
			client->set_open_handler([this](typename websocketpp::connection_hdl hdl)
			{
				m_MessageHandler->OnOpen();
			});
			client->set_close_handler([this](typename websocketpp::connection_hdl hdl)
			{
				m_MessageHandler->OnClose();
			});
			client->set_fail_handler([this](typename websocketpp::connection_hdl hdl)
			{
				m_MessageHandler->OnFail();
			});
			client->set_message_handler([this](typename websocketpp::connection_hdl hdl, MessageT message)
			{
				OnMessage(message->get_payload(), message->get_opcode());
			});
		}
		template<class ClientT, class ConnectionT> void CreateConnection()
		{
			ClientT* client = CastClient<ClientT>();

			std::error_code connectionErrorCode;
			ConnectionT connection = client->get_connection(m_URL, connectionErrorCode);
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
				std::thread asioThread(&ClientT::run, client);
				asioThread.detach();
			}
		}

	private:
		void OnMessage(const std::string& message, int opCode);
		void OnLogMessage(const char* message, int logLevel);
		void Destroy();
		bool DoClose(int code, const std::string& reason, int& errorCode);
		bool IsWSS(const std::string& url) const;

		MessageHandler* GetHandler() const
		{
			return m_MessageHandler;
		}

	public:
		KxWebSocketClientImpl(MessageHandler* handler, const std::string& url);
		~KxWebSocketClientImpl();

	public:
		bool IsOK() const;
		bool IsSecure() const
		{
			return m_IsSecure;
		}
		int GetCloseCode() const;

		bool SendBinary(const void* data, size_t length);
		bool SendText(const char* stringUTF8, size_t length);

		bool Close()
		{
			int errorCode = 0;
			return DoClose(0, std::string(), errorCode);
		}
		bool Close(int code, const std::string& reason = std::string())
		{
			int errorCode = 0;
			return DoClose(code, reason, errorCode);
		}
		bool Close(int code, const std::string& reason, int& errorCode)
		{
			return DoClose(code, reason, errorCode);
		}
};
