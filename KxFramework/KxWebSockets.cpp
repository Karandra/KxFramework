/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxWebSockets.h"
#include "KxFramework/KxWebSocketsImpl.h"
#include "KxFramework/KxINet.h"
#include "KxFramework/KxCrypto.h"

wxString KxWebSocketClient::GetLibraryName()
{
	return KxWebSocketClientImpl::GetLibraryName();
}
wxString KxWebSocketClient::GetLibraryVersion()
{
	return KxWebSocketClientImpl::GetLibraryVersion();
}

bool KxWebSocketClient::OnOpen()
{
	wxCriticalSectionLocker lock(m_CritSection);
	m_IsOpen = true;

	wxLogMessage(__FUNCTION__);

	KxWebSocketEvent event;
	CreateEventTemplate(event, KxEVT_WEBSOCKET_OPEN);
	ProcessEvent(event);
	return event.IsAllowed();
}
bool KxWebSocketClient::OnReady()
{
	wxLogMessage(__FUNCTION__);
	return true;
}
bool KxWebSocketClient::OnAccept()
{
	wxLogMessage(__FUNCTION__);
	return true;
}
void KxWebSocketClient::OnClose()
{
	wxCriticalSectionLocker lock(m_CritSection);
	m_IsDone = true;

	KxWebSocketEvent event;
	CreateEventTemplate(event, KxEVT_WEBSOCKET_CLOSE);
	ProcessEvent(event);
}
void KxWebSocketClient::OnFail()
{
	wxCriticalSectionLocker lock(m_CritSection);
	m_IsDone = true;

	KxWebSocketEvent event;
	CreateEventTemplate(event, KxEVT_WEBSOCKET_FAIL);
	ProcessEvent(event);
}

void KxWebSocketClient::OnTextMessage(const char* data, size_t dataLength)
{
	//wxLogMessage("[%s]: %s", __FUNCTION__, wxString(data, dataLength));

	KxWebSocketEvent event;
	CreateEventTemplate(event, KxEVT_WEBSOCKET_MESSAGE);
	event.SetString(wxString::FromUTF8(data, dataLength));

	ProcessEvent(event);
}
void KxWebSocketClient::OnBinaryMessage(const void* data, size_t dataLength)
{
	KxWebSocketEvent event;
	CreateEventTemplate(event, KxEVT_WEBSOCKET_MESSAGE);
	event.SetBinaryMessage(data, dataLength);
}

void KxWebSocketClient::OnLogDebug(const char* message)
{
	wxLogInfo("[KxWebSocketClient<Debug>]: %s", message);
}
void KxWebSocketClient::OnLogError(const char* message)
{
	wxLogError("[KxWebSocketClient]: %s", message);
}
void KxWebSocketClient::OnLogFatal(const char* message)
{
	wxLogFatalError("[KxWebSocketClient]: %s", message);
}

void KxWebSocketClient::CreateEventTemplate(KxWebSocketEvent& event, wxEventType type)
{
	event.SetEventType(type);
	event.SetEventObject(this);
}

KxWebSocketClient::KxWebSocketClient(const wxString& url)
{
	auto urlUTF8 = url.ToUTF8();
	m_Impl = new KxWebSocketClientImpl(this, urlUTF8.data());
}
KxWebSocketClient::~KxWebSocketClient()
{
	delete m_Impl;
}

bool KxWebSocketClient::IsOK() const
{
	return m_Impl && m_Impl->IsOK();
}
bool KxWebSocketClient::IsSecure() const
{
	return m_Impl->IsSecure();
}
int KxWebSocketClient::GetCloseCode() const
{
	return m_Impl->GetCloseCode();
}

bool KxWebSocketClient::Send(const void* data, size_t length)
{
	return m_Impl->SendBinary(data, length);
}
bool KxWebSocketClient::Send(const wxString& message)
{
	auto messageUTF8 = message.ToUTF8();
	return m_Impl->SendText(messageUTF8.data(), messageUTF8.length());
}

bool KxWebSocketClient::Close() const
{
	return m_Impl->Close();
}
bool KxWebSocketClient::Close(int code, const wxString& status) const
{
	auto statusUTF8 = status.ToUTF8();
	return m_Impl->Close(code, std::string(statusUTF8.data(), statusUTF8.length()));
}
bool KxWebSocketClient::Close(int code, const wxString& status, int& errorCode) const
{
	auto statusUTF8 = status.ToUTF8();
	return m_Impl->Close(code, std::string(statusUTF8.data(), statusUTF8.length()), errorCode);
}
