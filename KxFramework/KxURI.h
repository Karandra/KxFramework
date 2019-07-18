#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/uri.h>

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

namespace Kx::Network
{
	wxString NormalizeAddress(const wxString& address);
}

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
				return static_cast<const TBase&>(*m_URI);
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
		KxURI(const wxString& uri)
			:wxURI(Kx::Network::NormalizeAddress(uri)), THelper(this)
		{
		}
		KxURI(const char* uri)
			:KxURI(wxString(uri))
		{
		}
		KxURI(const wchar_t* uri)
			:KxURI(wxString(uri))
		{
		}
		KxURI(const wxURI& other)
			:KxURI(other.BuildURI())
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
			AsBase() = Kx::Network::NormalizeAddress(uri);
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
			AsBase() = Kx::Network::NormalizeAddress(other.BuildURI());
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
