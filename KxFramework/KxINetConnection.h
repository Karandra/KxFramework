/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxINetEvent.h"
class KX_API KxINet;
class KX_API KxINetRequest;

class KX_API KxINetConnection: public wxEvtHandler
{
	friend class KxINet;
	friend class KxINetConnectionThread;

	private:
		using HINTERNET = void*;
		using HeadersMap = std::unordered_map<wxString, wxString>;

	private:
		KxINet* m_SessionInstance = NULL;
		HINTERNET m_Handle = NULL;

		wxString m_URL;
		wxString m_UserName;
		wxString m_Password;
		KxINetURLParts m_URLParts;
		
		uint16_t m_Port = 0;
		KxINetService m_ServiceType = KxINET_SERVICE_NONE;

	private:
		bool SendQueryEvent(HINTERNET requestHandle);
		void AddRequestHeadres(const KxINetRequest& request, HINTERNET requestHandle);
		void QueryConnectionStatus(HINTERNET requestHandle, KxINetResponse& response);
		wxString GetRequestURL(const KxINetRequest& request) const;

	protected:
		virtual KxINetResponse DoSendRequest(const wxString& method, KxINetRequest& request, bool test);
		
	public:
		KxINetConnection(const KxINetConnection& other)
		{
			*this = other;
		}
		KxINetConnection(KxINet* session, const wxString& url, uint16_t port, const wxString& userName, const wxString& password);
		virtual ~KxINetConnection();

	public:
		KxINet* GetSession() const
		{
			return m_SessionInstance;
		}
		HINTERNET GetHandle() const
		{
			return m_Handle;
		}
		
		bool IsOK() const
		{
			return m_ServiceType != KxINET_SERVICE_NONE;
		}
		bool IsConnected() const
		{
			return m_Handle != NULL;
		}
		
		wxString GetURL() const;
		void SetURL(const wxString& url);

		bool Connect();
		bool CloseConnection();

		bool TestConnection(KxINetRequest& request, const wxString& method = wxEmptyString)
		{
			return DoSendRequest(method, request, true).StatusCode == 200;
		}
		KxINetResponse SendRequest(KxINetRequest& request, const wxString& method = wxEmptyString)
		{
			return DoSendRequest(method, request, false);
		}

	public:
		KxINetConnection& operator=(const KxINetConnection& other);
};
