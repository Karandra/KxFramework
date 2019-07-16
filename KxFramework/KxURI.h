#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/uri.h>
#include <wx/url.h>

enum class KxURIHostType: std::underlying_type_t<wxURIHostType>
{
	RegName = wxURI_REGNAME,
	IPv4 = wxURI_IPV4ADDRESS,
	IPv6 = wxURI_IPV6ADDRESS,
	IPvFuture = wxURI_IPVFUTURE
};
enum class KxURIFlags: std::underlying_type_t<wxURIFlags>
{
	None = 0,
	UseStrict = wxURI_STRICT
};
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

namespace Kx::Network
{
	template<class aTDerived, class aTBase> class URXHelper
	{
		protected:
			using TBase = aTBase;
			using TDerived = aTDerived;
			using THelper = URXHelper;

		private:
			wxURI* m_URI = nullptr;

		protected:
			TBase& AsBase()
			{
				return static_cast<TBase&>(*m_URI);
			}
			const TBase& AsBase() const
			{
				return static_cast<const TBase>(*m_URI);
			}
			
			TDerived& AsDerived()
			{
				return static_cast<TDerived&>(*m_URI);
			}
			const TDerived& AsDerived() const
			{
				return static_cast<const TDerived&>(*m_URI);
			}

		protected:
			URXHelper(wxURI* uri)
				:m_URI(uri)
			{
			}

		public:
			KxURIHostType GetHostType() const
			{
				return static_cast<KxURIHostType>(AsBase().GetHostType());
			}
			bool Resolve(const wxURI& base, KxURIFlags flags = KxURIFlags::UseStrict)
			{
				AsBase().Resolve(base, static_cast<int>(flags));
				return AsDerived().IsOk();
			}
			
		public:
			explicit operator bool() const
			{
				return AsDerived().IsOk();
			}
			bool operator!() const
			{
				return !AsDerived().IsOk();
			}
			
			bool operator==(const wxURL& other) const
			{
				return AsBase() == other;
			}
			bool operator!=(const wxURL& other) const
			{
				return !(*this == other);
			}

			bool operator==(const wxURI& other) const
			{
				return AsBase() == other;
			}
			bool operator!=(const wxURI& other) const
			{
				return !(*this == other);
			}
	};
}

class KX_API KxURI: public wxURI, public Kx::Network::URXHelper<KxURI, wxURI>
{
	public:
		KxURI()
			:THelper(this)
		{
		}
		KxURI(const wxString& url)
			:wxURI(url), THelper(this)
		{
		}
		KxURI(const char* url)
			:KxURI(wxString(url))
		{
		}
		KxURI(const wchar_t* url)
			:KxURI(wxString(url))
		{
		}
		KxURI(const wxURI& other)
			:wxURI(other), THelper(this)
		{
		}
		KxURI(const KxURI& other)
			:wxURI(other), THelper(this)
		{
		}
		KxURI(KxURI&& other)
			:wxURI(std::move(other)), THelper(this)
		{
			other.Clear();
		}

	public:
		bool IsOk() const
		{
			return m_fields != 0;
		}
		void Clear()
		{
			wxURI::Clear();
		}

		using THelper::GetHostType;
		using THelper::Resolve;

	public:
		TDerived& operator=(const wxString& uri)
		{
			AsBase() = uri;
			return AsDerived();
		}
		TDerived& operator=(const char* uri)
		{
			*this = wxString(uri);
			return AsDerived();
		}
		TDerived& operator=(const wchar_t* uri)
		{
			*this = wxString(uri);
			return AsDerived();
		}
		TDerived& operator=(const wxURI& other)
		{
			AsBase() = other;
			return AsDerived();
		}
		TDerived& operator=(const wxURL& other)
		{
			AsBase() = other;
			return AsDerived();
		}
		TDerived& operator=(const TDerived& other)
		{
			AsBase() = other;
			return AsDerived();
		}
		TDerived& operator=(TDerived&& other)
		{
			AsBase() = std::move(other);
			other.Clear();

			return AsDerived();
		}
};

class KX_API KxURL: public wxURL, public Kx::Network::URXHelper<KxURL, wxURL>
{
	public:
		KxURL()
			:THelper(this)
		{
		}
		KxURL(const wxString& url)
			:wxURL(url), THelper(this)
		{
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
		}
		KxURL(const wxURL& other)
			:wxURL(other), THelper(this)
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
			return static_cast<KxURLError>(wxURL::SetURL(url));
		}

		std::unique_ptr<wxInputStream> GetInputStream()
		{
			return std::unique_ptr<wxInputStream>(wxURL::GetInputStream());
		}

		using THelper::GetHostType;
		using THelper::Resolve;

	public:
		TDerived& operator=(const wxString& url)
		{
			AsBase() = url;
			return AsDerived();
		}
		TDerived& operator=(const char* url)
		{
			*this = wxString(url);
			return AsDerived();
		}
		TDerived& operator=(const wchar_t* url)
		{
			*this = wxString(url);
			return AsDerived();
		}
		TDerived& operator=(const wxURI& other)
		{
			AsBase() = other;
			return AsDerived();
		}
		TDerived& operator=(const wxURL& other)
		{
			AsBase() = other;
			return AsDerived();
		}
		TDerived& operator=(const TDerived& other)
		{
			AsBase() = other;
			return AsDerived();
		}
		TDerived& operator=(TDerived&& other)
		{
			AsBase() = std::move(other);
			other.Clear();

			return AsDerived();
		}
};
