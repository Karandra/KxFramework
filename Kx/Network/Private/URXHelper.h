#pragma once
#include "../Common.h"
#include <wx/uri.h>
#include <wx/url.h>

namespace KxFramework
{
	class URI;
	class URL;
}

namespace KxFramework::Network::Private
{
	inline String NormalizeInputAddress(const String& address)
	{
		if (!address.IsEmpty())
		{
			String temp = address.Clone();
			temp.Trim();
			temp.Trim(StringOpFlag::FromEnd);
			temp.Replace(wxS('\\'), wxS('/'));

			return temp;
		}
		return address;
	}
	inline void NormalizeURI(wxString& scheme,
							 wxString& server,
							 wxString& path,
							 wxString& query,
							 wxString& fragment,
							 wxString& userInfo,
							 wxString& port
	)
	{
		scheme.MakeLower();
		server.MakeLower();
		path.Replace(wxS('\\'), wxS('/'));
	}

	constexpr NetworkHostType MapHostType(wxURIHostType hostTypeWx) noexcept
	{
		switch (hostTypeWx)
		{
			case wxURIHostType::wxURI_REGNAME:
			{
				return NetworkHostType::RegName;
			}
			case wxURIHostType::wxURI_IPV4ADDRESS:
			{
				return NetworkHostType::IPv4;
			}
			case wxURIHostType::wxURI_IPV6ADDRESS:
			{
				return NetworkHostType::IPv6;
			}
			case wxURIHostType::wxURI_IPVFUTURE:
			{
				return NetworkHostType::IPvUnknown;
			}
		}
		return NetworkHostType::None;
	}
	constexpr URLStatus MapURLStatus(wxURLError urlError) noexcept
	{
		switch (urlError)
		{
			case wxURLError::wxURL_NOERR:
			{
				return URLStatus::Success;
			}
			case wxURLError::wxURL_CONNERR:
			{
				return URLStatus::ConnectionError;
			}
			case wxURLError::wxURL_NOHOST:
			{
				return URLStatus::NoHost;
			}
			case wxURLError::wxURL_NOPATH:
			{
				return URLStatus::NoPath;
			}
			case wxURLError::wxURL_NOPROTO:
			{
				return URLStatus::NoProtocol;
			}
			case wxURLError::wxURL_PROTOERR:
			{
				return URLStatus::ProtocolError;
			}
			case wxURLError::wxURL_SNTXERR:
			{
				return URLStatus::Syntax;
			}
		}
		return URLStatus::Unknown;
	}
}

namespace KxFramework::Network::Private
{
	template<class TDerived_, class TBase_>
	class URXHelper
	{
		protected:
			using TBase = TBase_;
			using TDerived = TDerived_;
			using THelper = URXHelper;

		private:
			wxURI* m_URI = nullptr;

		protected:
			TBase& AsBase() noexcept
			{
				return static_cast<TBase&>(*m_URI);
			}
			const TBase& AsBase() const noexcept
			{
				return static_cast<const TBase&>(*m_URI);
			}
			
			TDerived& AsDerived() noexcept
			{
				return static_cast<TDerived&>(*m_URI);
			}
			const TDerived& AsDerived() const noexcept
			{
				return static_cast<const TDerived&>(*m_URI);
			}

		protected:
			URXHelper(wxURI* uri) noexcept
				:m_URI(uri)
			{
			}
			~URXHelper() noexcept = default;

		public:
			NetworkHostType GetHostType() const noexcept
			{
				return MapHostType(AsBase().GetHostType());
			}
			bool Resolve(const URI& base, FlagSet<URIFlag> flags = URIFlag::Strict)
			{
				AsBase().Resolve(base, flags & URIFlag::Strict ? wxURI_STRICT : 0);
				return !AsDerived().IsNull();
			}
			
			String BuildURI() const
			{
				return AsBase().BuildURI();
			}
			String BuildUnescapedURI() const
			{
				return AsBase().BuildUnescapedURI();
			}

			String GetFragment() const
			{
				return AsBase().GetFragment();
			}
			String GetPassword() const
			{
				return AsBase().GetPassword();
			}
			String GetPath() const
			{
				return AsBase().GetPath();
			}
			String GetPort() const
			{
				return AsBase().GetPort();
			}
			String GetQuery() const
			{
				return AsBase().GetQuery();
			}
			String GetScheme() const
			{
				return AsBase().GetScheme();
			}
			String GetServer() const
			{
				return AsBase().GetServer();
			}
			String GetUser() const
			{
				return AsBase().GetUser();
			}
			String GetUserInfo() const
			{
				return AsBase().GetUserInfo();
			}

		public:
			explicit operator bool() const noexcept
			{
				return !AsDerived().IsNull();
			}
			bool operator!() const noexcept
			{
				return AsDerived().IsNull();
			}

			bool operator==(const wxURI& other) const noexcept
			{
				return AsBase() == other;
			}
			bool operator!=(const wxURI& other) const noexcept
			{
				return !(*this == other);
			}
	};
}
