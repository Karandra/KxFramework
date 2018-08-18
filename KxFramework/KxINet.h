#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxINetEvent.h"
#include "KxFramework/KxINetRequest.h"
#include "KxFramework/KxINetConnection.h"

class KxINet
{
	friend class KxINetConnection;

	private:
		static const WCHAR* AcceptTypes[];

	public:
		static const DWORD DefaultTimeout = 20000;
		static const DWORD TransferBlockSize = 64 * 1024;
		static const uint16_t DefaultPort;
		static const uint16_t DefaultPortHTTP;
		static const uint16_t DefaultPortHTTPS;

	public:
		static LPCWSTR StringOrNull(const wxString& string)
		{
			return string.IsEmpty() ? NULL : string.wc_str();
		}
		static KxINetURLParts SplitURL(const wxString& url);
		static bool IsInternetAvailable();
		static wxString LookupIP(const wxString& url);

	private:
		using HINTERNET = void*;

		HINTERNET m_Handle = NULL;
		DWORD m_TimeOut = DefaultTimeout;

		std::list<std::unique_ptr<KxINetConnection>> m_ConnectionsList;

	private:
		void SetTimeouts();

	protected:
		virtual KxINetConnection* OnMakeConnection(const wxString& url, uint16_t port, const wxString& userName, const wxString& password);
		virtual wxString OnGetUserAgent() const;

	public:
		KxINet(DWORD timeout = DefaultTimeout);
		virtual ~KxINet();

	public:
		HINTERNET GetHandle() const
		{
			return m_Handle;
		}
		
		void DestroyConnection(KxINetConnection& connection);
		KxINetConnection& MakeConnection(const wxString& url, uint16_t port = DefaultPort, const wxString& userName = wxEmptyString, const wxString& password = wxEmptyString);
		template<class T = KxINetConnection> T CreateConnection(const wxString& url, uint16_t port = DefaultPort, const wxString& userName = wxEmptyString, const wxString& password = wxEmptyString)
		{
			return T(this, url, port, userName, password);
		}
};
