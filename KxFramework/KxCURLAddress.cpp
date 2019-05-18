#include "KxStdAfx.h"
#include "KxFramework/KxCURLAddress.h"
#include "KxFramework/KxUtility.h"

#define CURL_STATICLIB 1
#include "KxFramework/cURL/urlapi.h"

namespace
{
	constexpr CURLUPart ToCURLUPart(KxCURLAddressPart part)
	{
		switch (part)
		{
			case KxCURLAddressPart::URL:
			{
				return CURLUPART_URL;
			}
			case KxCURLAddressPart::Scheme:
			{
				return CURLUPART_SCHEME;
			}
			case KxCURLAddressPart::User:
			{
				return CURLUPART_USER;
			}
			case KxCURLAddressPart::Password:
			{
				return CURLUPART_PASSWORD;
			}
			case KxCURLAddressPart::Options:
			{
				return CURLUPART_OPTIONS;
			}
			case KxCURLAddressPart::Host:
			{
				return CURLUPART_HOST;
			}
			case KxCURLAddressPart::Port:
			{
				return CURLUPART_PORT;
			}
			case KxCURLAddressPart::Path:
			{
				return CURLUPART_PATH;
			}
			case KxCURLAddressPart::Query:
			{
				return CURLUPART_QUERY;
			}
			case KxCURLAddressPart::Fragment:
			{
				return CURLUPART_FRAGMENT;
			}
		};
		return CURLUPART_URL;
	}
	constexpr uint32_t ToCURLUFlags(KxCURLAddressFlag flags)
	{
		uint32_t curluFlags = 0;

		// Get
		KxUtility::ModFlagRef(curluFlags, CURLU_URLDECODE, flags & KxCURLAddressFlag::DecodeURL);
		KxUtility::ModFlagRef(curluFlags, CURLU_DEFAULT_PORT, flags & KxCURLAddressFlag::DefaultPort);
		KxUtility::ModFlagRef(curluFlags, CURLU_NO_DEFAULT_PORT, flags & KxCURLAddressFlag::NoDefaultPort);

		// Set
		KxUtility::ModFlagRef(curluFlags, CURLU_URLENCODE, flags & KxCURLAddressFlag::EncodeURL);
		KxUtility::ModFlagRef(curluFlags, CURLU_GUESS_SCHEME, flags & KxCURLAddressFlag::GuessScheme);
		KxUtility::ModFlagRef(curluFlags, CURLU_NON_SUPPORT_SCHEME, flags & KxCURLAddressFlag::NonSupportScheme);

		// Get, Set
		KxUtility::ModFlagRef(curluFlags, CURLU_DEFAULT_SCHEME, flags & KxCURLAddressFlag::DefaultScheme);

		return curluFlags;
	}
}

void KxCURLAddress::Create()
{
	m_Handle = curl_url();
}
void KxCURLAddress::Destroy()
{
	if (m_Handle)
	{
		curl_url_cleanup(m_Handle);
		m_Handle = nullptr;
	}
}
Curl_URL* KxCURLAddress::CopyFrom(const KxCURLAddress& other)
{
	if (other.IsOK())
	{
		return curl_url_dup(other.m_Handle);
	}
	return nullptr;
}

wxString KxCURLAddress::DoGet(Part part, Flag flags) const
{
	const CURLUPart curluPart = ToCURLUPart(part);
	const uint32_t curluFlags = ToCURLUFlags(flags);

	char* value = nullptr;
	if (curl_url_get(m_Handle, curluPart, &value, curluFlags) == CURLUE_OK)
	{
		wxString ret = wxString::FromUTF8(value);
		curl_free(value);
		return ret;
	}
	return {};
}
bool KxCURLAddress::DoSet(Part part, const wxString& value, Flag flags)
{
	const CURLUPart curluPart = ToCURLUPart(part);
	const uint32_t curluFlags = ToCURLUFlags(flags);

	if (value.IsEmpty())
	{
		return curl_url_set(m_Handle, curluPart, nullptr, curluFlags) == CURLUE_OK;
	}
	else
	{
		auto utf8 = value.ToUTF8();
		return curl_url_set(m_Handle, curluPart, utf8.data(), curluFlags) == CURLUE_OK;
	}
}
