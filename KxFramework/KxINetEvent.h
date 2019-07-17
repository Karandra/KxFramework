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
class KX_API KxINetConnection;
class KX_API KxINetEvent: public wxNotifyEvent
{
	friend class KxINetConnection;
	using HINTERNET = void*;

	private:
		KxINetConnection* m_Connection = nullptr;
		HINTERNET m_RequestHandle = nullptr;

		const void* m_RequestBuffer = nullptr;
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
		KxINetEvent(wxEventType type = wxEVT_NULL, KxINetConnection* object = nullptr);
		virtual ~KxINetEvent();
		virtual KxINetEvent* Clone() const;

	public:
		bool IsOK() const
		{
			return m_Connection != nullptr;
		}
		KxINetConnection& GetConnection() const
		{
			return *m_Connection;
		}

		wxString QueryInfoString(DWORD infoLevel, DWORD index = 0, DWORD* nextIndex = nullptr) const;
		int64_t QueryInfoNumber(DWORD infoLevel, DWORD index = 0, DWORD* nextIndex = nullptr) const;
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
KxEVENT_DECLARE_GLOBAL(INET_SEND_DATA, KxINetEvent);
KxEVENT_DECLARE_GLOBAL(INET_QUERY_INFO, KxINetEvent);
KxEVENT_DECLARE_GLOBAL(INET_DOWNLOAD, KxFileOperationEvent);
