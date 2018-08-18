#pragma once
#include <string>
class KxWebSocketClientImpl;

class KxWebSocketsClientImplMessageHandler
{
	friend class KxWebSocketClientImpl;

	protected:
		virtual bool OnOpen()
		{
			return true;
		}
		virtual bool OnReady()
		{
			return true;
		}
		virtual bool OnAccept()
		{
			return true;
		}
		virtual void OnClose() {}
		virtual void OnFail() {}

		virtual void OnTextMessage(const char* data, size_t dataLength) {}
		virtual void OnBinaryMessage(const void* data, size_t dataLength) {}

		virtual void OnLogDebug(const char* message) {}
		virtual void OnLogError(const char* message) {}
		virtual void OnLogFatal(const char* message) {}

	public:
		void OnLogDebug(const std::string& message)
		{
			OnLogDebug(message.c_str());
		}
		void OnLogError(const std::string& message)
		{
			OnLogError(message.c_str());
		}
		void OnLogFatal(const std::string& message)
		{
			OnLogFatal(message.c_str());
		}
};
