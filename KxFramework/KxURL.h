#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxURI.h"
#include <wx/url.h>

enum class KxURLError: std::underlying_type_t<wxURLError>
{
	Success = wxURL_NOERR,
	Syntax = wxURL_SNTXERR,
	NoProtocol = wxURL_NOPROTO,
	NoHost = wxURL_NOHOST,
	NoPath = wxURL_NOPATH,
	ConnectionError = wxURL_CONNERR,
	ProtocolError = wxURL_PROTOERR
};

class KX_API KxURL: public wxURL, public Kx::Network::URXHelper<KxURL, wxURL>
{
	protected:
		bool IsHTTPS() const;
		void OnAssignAddress();
		void NormalizeURI()
		{
			Kx::Network::NormalizeURI(m_scheme, m_server, m_path, m_query, m_fragment, m_userinfo, m_port);
		}

	public:
		KxURL()
			:THelper(this)
		{
		}
		KxURL(const wxString& url)
			:wxURL(Kx::Network::NormalizeInputAddress(url)), THelper(this)
		{
			NormalizeURI();
			OnAssignAddress();
		}
		KxURL(const char* url)
			:KxURL(wxString(url))
		{
		}
		KxURL(const wchar_t* url)
			:KxURL(wxString(url))
		{
		}
		KxURL(const wxURI& other)
			:KxURL(other.BuildURI())
		{
		}
		KxURL(const KxURL& other)
			:wxURL(other), THelper(this)
		{
		}
		KxURL(KxURL&& other)
			:wxURL(std::move(other)), THelper(this)
		{
			other.Clear();
		}

	public:
		bool IsOk() const
		{
			return m_fields != 0 && wxURL::IsOk();
		}
		void Clear()
		{
			AsBase() = wxURL();
		}

		KxURLError GetError() const
		{
			return static_cast<KxURLError>(wxURL::GetError());
		}
		KxURLError SetURL(const wxString& url)
		{
			wxURL::SetURL(Kx::Network::NormalizeInputAddress(url));
			NormalizeURI();
			OnAssignAddress();

			return GetError();
		}

		std::unique_ptr<wxInputStream> GetInputStream();

		using THelper::GetHostType;
		using THelper::Resolve;

	public:
		KxURL& operator=(const wxString& url)
		{
			SetURL(url);
			return *this;
		}
		KxURL& operator=(const char* url)
		{
			*this = wxString(url);
			return *this;
		}
		KxURL& operator=(const wchar_t* url)
		{
			*this = wxString(url);
			return *this;
		}
		KxURL& operator=(const wxURI& other)
		{
			*this = other.BuildURI();
			return *this;
		}
		KxURL& operator=(const KxURL& other)
		{
			AsBase() = other;
			return *this;
		}
		KxURL& operator=(KxURL&& other)
		{
			AsBase() = std::move(other);
			other.Clear();

			return *this;
		}
};
