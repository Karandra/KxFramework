#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileOperationEvent.h"

enum KxINetScheme
{
	KxINET_SCHEME_UNKNOWN,

	KxINET_SCHEME_HTTP,
	KxINET_SCHEME_HTTPS,
	KxINET_SCHEME_FTP,
};
enum KxINetService
{
	KxINET_SERVICE_NONE = 0,

	KxINET_SERVICE_HTTP,
	KxINET_SERVICE_FTP,
};

struct KxINetURLParts
{
	public:
		wxString HostName;
		wxString Sheme;
		wxString Path;
		wxString FullPath;
		wxString FileName;
		wxString ExtraInfo;
		wxString UserName;
		wxString Password;
		uint16_t Port = 0;
		KxINetScheme ShemeType = KxINET_SCHEME_UNKNOWN;

	public:
		KxINetURLParts();
};
struct KxINetResponse
{
	public:
		wxString Response;
		wxString Status;
		DWORD StatusCode = 0;
		DWORD ContentLength = (DWORD)-1;
		wxMemoryBuffer Data;

	public:
		KxINetResponse()
			:Data(0)
		{
		}
};

//////////////////////////////////////////////////////////////////////////
class KxINetConnection;
class KxINetEvent: public wxNotifyEvent
{
	friend class KxINetConnection;
	using HINTERNET = void*;

	private:
		KxINetConnection* m_Connection = NULL;
		HINTERNET m_RequestHandle = NULL;

		const void* m_RequestBuffer = NULL;
		DWORD m_RequestBufferSize = 0;

	private:
		HINTERNET GetRequestHandle() const
		{
			return m_RequestHandle;
		}
		void SetRequestHandle(HINTERNET handle)
		{
			m_RequestHandle = handle;
		}

	public:
		KxINetEvent(wxEventType type = wxEVT_NULL, KxINetConnection* object = NULL);
		virtual ~KxINetEvent();
		virtual KxINetEvent* Clone() const;

	public:
		bool IsOK() const
		{
			return m_Connection != NULL;
		}
		KxINetConnection& GetConnection() const
		{
			return *m_Connection;
		}

		wxString QueryInfoString(DWORD infoLevel, DWORD index = 0, DWORD* nextIndex = NULL) const;
		int64_t QueryInfoNumber(DWORD infoLevel, DWORD index = 0, DWORD* nextIndex = NULL) const;
		wxString QueryRequestURL() const;

		const void* GetRequestBuffer() const
		{
			return m_RequestBuffer;
		}
		DWORD GetRequestBufferSize() const
		{
			return m_RequestBufferSize;
		}
		void SetRequestBuffer(const void* buffer, DWORD bufferSize)
		{
			m_RequestBuffer = buffer;
			m_RequestBufferSize = bufferSize;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxINetEvent);
};

//////////////////////////////////////////////////////////////////////////
wxDECLARE_EVENT(KxEVT_INET_SEND_DATA, KxINetEvent);
wxDECLARE_EVENT(KxEVT_INET_QUERY_INFO, KxINetEvent);
wxDECLARE_EVENT(KxEVT_INET_DOWNLOAD, KxFileOperationEvent);
