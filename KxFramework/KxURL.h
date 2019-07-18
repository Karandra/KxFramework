#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxURI.h"
#include <wx/url.h>

class KX_API KxURL: public wxURL, public Kx::Network::URXHelper<KxURL, wxURL>
{
	private:
		void OnAssignAddress();

	public:
		KxURL()
			:THelper(this)
		{
		}
		KxURL(const wxString& url)
			:wxURL(url), THelper(this)
		{
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
			:wxURL(other), THelper(this)
		{
			OnAssignAddress();
		}
		KxURL(const wxURL& other)
			:wxURL(other), THelper(this)
		{
			OnAssignAddress();
		}
		KxURL(const KxURL& other)
			:wxURL(other), THelper(this)
		{
			OnAssignAddress();
		}
		KxURL(KxURL&& other)
			:wxURL(std::move(other)), THelper(this)
		{
			other.Clear();
			OnAssignAddress();
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
			wxURL::SetURL(url);
			OnAssignAddress();
			return GetError();
		}

		std::unique_ptr<wxInputStream> GetInputStream();

		using THelper::GetHostType;
		using THelper::Resolve;

	public:
		TDerived& operator=(const wxString& url)
		{
			AsBase() = url;
			OnAssignAddress();
			return AsDerived();
		}
		TDerived& operator=(const char* url)
		{
			*this = wxString(url);
			OnAssignAddress();
			return AsDerived();
		}
		TDerived& operator=(const wchar_t* url)
		{
			*this = wxString(url);
			OnAssignAddress();
			return AsDerived();
		}
		TDerived& operator=(const wxURI& other)
		{
			AsBase() = other;
			OnAssignAddress();
			return AsDerived();
		}
		TDerived& operator=(const wxURL& other)
		{
			AsBase() = other;
			OnAssignAddress();
			return AsDerived();
		}
		TDerived& operator=(const TDerived& other)
		{
			AsBase() = other;
			OnAssignAddress();
			return AsDerived();
		}
		TDerived& operator=(TDerived&& other)
		{
			AsBase() = std::move(other);
			OnAssignAddress();
			other.Clear();

			return AsDerived();
		}
};
