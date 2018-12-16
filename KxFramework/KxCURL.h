/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxSingleton.h"
#include "KxFramework/KxCURLStructs.h"
#include "KxFramework/KxCURLEvent.h"

class KX_API KxCURL: public KxSingleton<KxCURL>
{
	friend class KxCURLModule;

	public:
		static wxString GetVersion();

	private:
		bool m_IsInitialized = false;

	public:
		KxCURL();
		~KxCURL();

	public:
		bool IsInitialized() const
		{
			return m_IsInitialized;
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxCURLSession: public wxEvtHandler
{
	using CURL = void;
	class CallbackData
	{
		private:
			KxCURLSession& m_Session;
			KxCURLReplyBase& m_Reply;

		public:
			CallbackData(KxCURLSession& session, KxCURLReplyBase& reply)
				:m_Session(session), m_Reply(reply)
			{
			}

		public:
			KxCURLSession& GetSession()
			{
				return m_Session;
			}
			KxCURLReplyBase& GetReply()
			{
				return m_Reply;
			}
	};

	private:
		CURL* m_Handle = NULL;
		bool m_IsPaused = false;
		bool m_IsStopped = false;
		
		KxStdStringVector m_Headers;
		void* m_HeadersSList = NULL;

		wxString m_URL;
		wxString m_PostData;

	private:
		static size_t OnWriteResponse(char* data, size_t size, size_t count, void* userData);

	private:
		int SetOption(int option, const wxString& value, size_t* length = NULL);
		int SetOption(int option, int value);
		int SetOption(int option, int64_t value);
		int SetOption(int option, size_t value);
		int SetOption(int option, bool value);
		int SetOption(int option, const void* value);
		template<class T> int SetOptionFunction(int option, T value)
		{
			return SetOption(option, reinterpret_cast<const void*>(value));
		}

		void SetHeaders();
		void DoSendRequest(KxCURLReplyBase& reply);

	public:
		KxCURLSession(const wxString& url = wxEmptyString);
		virtual ~KxCURLSession();

	public:
		void Close();
		CURL* GetHandle() const
		{
			return m_Handle;
		}

		KxCURLReply Send()
		{
			KxCURLReply reply;
			DoSendRequest(reply);
			return reply;
		}
		KxCURLBinaryReply Download()
		{
			KxCURLBinaryReply reply;
			DoSendRequest(reply);
			return reply;
		}
		void Download(KxCURLStreamReply& reply);

		KxStringVector GetReplyCookies() const;

		bool IsPaused() const
		{
			return m_IsPaused;
		}
		bool Pause();
		bool Resume();

		bool IsStopped() const
		{
			return m_IsStopped;
		}
		void Stop();

		void SetURL(const wxString& url)
		{
			m_URL = url;
		}
		void SetPostData(const wxString& data)
		{
			m_PostData = data;
		}

		void ClearHeaders()
		{
			m_Headers.clear();
		}
		void AddHeader(const wxString& name, const wxString& value);
		void AddHeader(const wxString& value);
};
