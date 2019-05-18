/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
struct Curl_URL;
class KxCURLSession;

enum class KxCURLAddressPart: uint32_t
{
	URL,
	Scheme,
	User,
	Password,
	Options,
	Host,
	//ZoneID, v7.65.0+ required
	Port,
	Path,
	Query,
	Fragment
};

enum class KxCURLAddressFlag: uint32_t
{
	None = 0,

	EncodeURL = 1 << 0, // EncodeURL: [Set]
	DecodeURL = 1 << 1, // DecodeURL: [Get]

	DefaultScheme = 1 << 2, // DefaultScheme: [Get, Set]
	NonSupportScheme = 1 << 3, // NonSupportScheme: [Set]
	GuessScheme = 1 << 4, // GuessScheme: [Set]

	DefaultPort = 1 << 5, // DefaultPort: [Get]
	NoDefaultPort = 1 << 6, // NoDefaultPort: [Get]
};
inline constexpr KxCURLAddressFlag operator|(KxCURLAddressFlag left, KxCURLAddressFlag right)
{
	using T = std::underlying_type_t<KxCURLAddressFlag>;
	return static_cast<KxCURLAddressFlag>(static_cast<T>(left) | static_cast<T>(right));
}
inline constexpr bool operator&(KxCURLAddressFlag left, KxCURLAddressFlag right)
{
	using T = std::underlying_type_t<KxCURLAddressFlag>;
	return (static_cast<T>(left) & static_cast<T>(right)) != 0;
}

class KX_API KxCURLAddress
{
	friend class KxCURLSession;

	public:
		using Part = KxCURLAddressPart;
		using Flag = KxCURLAddressFlag;

	private:
		Curl_URL* m_Handle = nullptr;

	private:
		void Create();
		void Destroy();
		Curl_URL* CopyFrom(const KxCURLAddress& other);

		wxString DoGet(Part part, Flag flags = Flag::None) const;
		bool DoSet(Part part, const wxString& value, Flag flags = Flag::None);

	public:
		KxCURLAddress() = default;
		KxCURLAddress(Part part, const wxString& value, Flag flags = Flag::None)
		{
			Create();
			Set(part, value, flags);
		}
		KxCURLAddress(const wxString& url, Flag flags = Flag::None)
		{
			Create();
			SetURL(url, flags);
		}
		
		KxCURLAddress(const KxCURLAddress& other)
		{
			Create();
			*this = other;
		}
		KxCURLAddress(KxCURLAddress&& other)
		{
			Create();
			*this = std::move(other);
		}
		~KxCURLAddress()
		{
			Destroy();
		}

	public:
		bool IsOK() const
		{
			return m_Handle != nullptr;
		}
		explicit operator bool() const
		{
			return IsOK();
		}
		bool operator!() const
		{
			return !IsOK();
		}

		wxString Get(Part part, Flag flags = Flag::None) const
		{
			return DoGet(part, flags);
		}
		wxString GetURL(Flag flags = Flag::None) const
		{
			return DoGet(Part::URL, flags);
		}
		wxString GetScheme(Flag flags = Flag::None) const
		{
			return DoGet(Part::Scheme, flags);
		}
		wxString GetHost(Flag flags = Flag::None) const
		{
			return DoGet(Part::Host, flags);
		}
		wxString GetPath(Flag flags = Flag::None) const
		{
			return DoGet(Part::Path, flags);
		}
		wxString GetQuery(Flag flags = Flag::None) const
		{
			return DoGet(Part::Query, flags);
		}

		bool Set(Part part, const wxString& value, Flag flags = Flag::None)
		{
			return DoSet(part, value, flags);
		}
		bool SetURL(const wxString& value, Flag flags = Flag::None)
		{
			return DoSet(Part::URL, value, flags);
		}
		bool SetScheme(const wxString& value, Flag flags = Flag::None)
		{
			return DoSet(Part::Scheme, value, flags);
		}
		bool SetHost(const wxString& value, Flag flags = Flag::None)
		{
			return DoSet(Part::Host, value, flags);
		}
		bool SetPath(const wxString& value, Flag flags = Flag::None)
		{
			return DoSet(Part::Path, value, flags);
		}
		bool SetQuery(const wxString& value, Flag flags = Flag::None)
		{
			return DoSet(Part::Query, value, flags);
		}

	public:
		KxCURLAddress& operator=(const KxCURLAddress& other)
		{
			Destroy();
			m_Handle = CopyFrom(other);

			return *this;
		}
		KxCURLAddress& operator=(KxCURLAddress&& other)
		{
			Destroy();
			std::swap(m_Handle, other.m_Handle);

			return *this;
		}
};
