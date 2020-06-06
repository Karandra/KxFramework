#pragma once
#include "../Common.h"
#include "../URI.h"
#include "Event.h"
#include "CloseCode.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	class KX_API IWebSocketClient: public wxEvtHandler, public RTTI::Interface<IWebSocketClient>
	{
		KxDeclareIID(IWebSocketClient, {0x42c097c9, 0x4c3e, 0x45fc, {0x98, 0xc6, 0x6e, 0xeb, 0x21, 0xc5, 0xa3, 0xd1}});

		public:
			using CloseCode = WebSocket::CloseCode;

		protected:
			virtual bool DoSendData(const void* data, size_t length) = 0;
			virtual bool DoSendData(const String& stringData) = 0;
			virtual void DoClose(CloseCode code, const String& status, int& errorCode) = 0;
			virtual bool DoConnect(const URI& address) = 0;

		public:
			virtual ~IWebSocketClient() = default;

		public:
			virtual bool IsConnected() const = 0;
			virtual bool IsSecure() const = 0;

			virtual CloseCode GetCloseCode() const = 0;
			virtual String GetCloseReason() const = 0;
			
			virtual void AddHeader(const String& key, const String& value) = 0;
			virtual void ReplaceHeader(const String& key, const String& value) = 0;
			virtual void RemoveHeader(const String& key) = 0;
			virtual void ClearHeaders() = 0;
			virtual void SetUserAgent(const String& userAgent) = 0;

			bool Connect(const URI& address = {})
			{
				return DoConnect(address);
			}

			bool Send(const void* data, size_t length)
			{
				return DoSendData(data, length);
			}
			bool Send(const String& message)
			{
				auto utf8 = message.ToUTF8();
				return DoSendData(utf8.data(), utf8.length());
			}
		
			int Close()
			{
				int errorCode = 0;
				DoClose(CloseCode::Normal, {}, errorCode);
				return errorCode;
			}
			int Close(const String& status)
			{
				int errorCode = 0;
				DoClose(CloseCode::Normal, status, errorCode);
				return errorCode;
			}
			int Close(CloseCode code, const String& status)
			{
				int errorCode = 0;
				DoClose(code, status, errorCode);
				return errorCode;
			}
	};
}
