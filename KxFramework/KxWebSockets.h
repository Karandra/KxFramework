#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWebSocketEvent.h"
#include "KxFramework/KxWebSocketsConstants.h"
#include "KxFramework/KxWebSocketsImplHandler.h"

class KxWebSocketClientImpl;
class KxWebSocketClient: public wxEvtHandler, public KxWebSocketsClientImplMessageHandler
{
	public:
		static wxString GetVersion();

	private:
		KxWebSocketClientImpl* m_Impl = NULL;

		wxCriticalSection m_CritSection;
		bool m_IsOpen = false;
		bool m_IsDone = false;

	private:
		virtual bool OnOpen() override;
		virtual bool OnReady() override;
		virtual bool OnAccept() override;
		virtual void OnClose() override;
		virtual void OnFail() override;

		virtual void OnTextMessage(const char* data, size_t dataLength) override;
		virtual void OnBinaryMessage(const void* data, size_t dataLength) override;

		virtual void OnLogDebug(const char* message) override;
		virtual void OnLogError(const char* message) override;
		virtual void OnLogFatal(const char* message) override;

	protected:
		void CreateEventTemplate(KxWebSocketEvent& event, wxEventType type);

	public:
		KxWebSocketClient(const wxString& url);
		virtual ~KxWebSocketClient();

	public:
		bool IsOK() const;
		bool IsSecure() const;
		int GetCloseCode() const;

		bool Send(const void* data, size_t length);
		bool Send(const wxMemoryBuffer& buffer)
		{
			return Send(buffer.GetData(), buffer.GetDataLen());
		}
		bool Send(const wxString& message);

		bool Close() const;
		bool Close(int code, const wxString& status = wxEmptyString) const;
		bool Close(int code, const wxString& status, int& errorCode) const;
};
