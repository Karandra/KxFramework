/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWebSocketEvent.h"
#include "Common.h"

namespace KxWebSocket
{
	class KX_API IClient: public wxEvtHandler
	{
		protected:
			virtual bool DoSendData(const void* data, size_t length) = 0;
			virtual bool DoSendData(const wxString& stringData) = 0;
			virtual void DoClose(CloseCode code, const wxString& status, int& errorCode) = 0;
			virtual bool DoConnect(const wxString& address) = 0;

		public:
			virtual ~IClient() = default;

		public:
			virtual bool IsOK() const = 0;
			virtual bool IsConnected() const = 0;
			virtual bool IsSecure() const = 0;

			virtual CloseCode GetCloseCode() const = 0;
			virtual wxString GetCloseReason() const = 0;
			
			virtual void AddHeader(const wxString& key, const wxString& value) = 0;
			virtual void ReplaceHeader(const wxString& key, const wxString& value) = 0;
			virtual void RemoveHeader(const wxString& key) = 0;
			virtual void ClearHeaders() = 0;
			virtual void SetUserAgent(const wxString& userAgent) = 0;

			bool Connect(const wxString& address = {})
			{
				return DoConnect(address);
			}

			bool Send(const void* data, size_t length)
			{
				return DoSendData(data, length);
			}
			bool Send(const wxString& message)
			{
				auto utf8 = message.ToUTF8();
				return DoSendData(utf8.data(), utf8.length());
			}
	
			void Close()
			{
				int errorCode = 0;
				DoClose(CloseCode::Normal, {}, errorCode);
			}
			void Close(const wxString& status)
			{
				int errorCode = 0;
				DoClose(CloseCode::Normal, status, errorCode);
			}
			void Close(CloseCode code, const wxString& status, int& errorCode)
			{
				DoClose(code, status, errorCode);
			}
	};
}
