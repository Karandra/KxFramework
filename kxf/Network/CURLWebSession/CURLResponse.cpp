#include "KxfPCH.h"
#include "CURLResponse.h"
#include "CURLRequest.h"
#include "CURLUtility.h"
#include "CURL.h"
#include "kxf/General/RegEx.h"
#include "kxf/IO/NullStream.h"
#include "kxf/FileSystem/FSPath.h"

#include "kxf/System/HandlePtr.h"
#include "kxf/Utility/Container.h"
#include "kxf/Utility/Enumerator.h"

namespace kxf
{
	CURL::Private::RequestHandle& CURLResponse::GetRequestHandle() const noexcept
	{
		return m_Request.m_Handle;
	}

	URI CURLResponse::GetURI() const
	{
		if (m_Request.m_FollowLocation == WebRequestOption2::Enabled)
		{
			return GetRequestHandle().GetOptionString(CURLINFO_EFFECTIVE_URL).value_or(NullString);
		}
		else
		{
			return GetRequestHandle().GetOptionString(CURLINFO_REDIRECT_URL).value_or(NullString);
		}
	}
	String CURLResponse::GetMethod() const
	{
		return GetRequestHandle().GetOptionString(CURLINFO_EFFECTIVE_METHOD).value_or(NullString);
	}
	String CURLResponse::GetPrimaryIP() const
	{
		return GetRequestHandle().GetOptionString(CURLINFO_PRIMARY_IP).value_or(NullString);
	}
	std::optional<uint16_t> CURLResponse::GetPrimaryPort() const
	{
		if (auto port = GetRequestHandle().GetOptionUInt32(CURLINFO_PRIMARY_PORT))
		{
			return static_cast<uint16_t>(*port);
		}
		return {};
	}
	WebRequestProtocol CURLResponse::GetProtocol() const
	{
		if (auto protocol = GetRequestHandle().GetOptionUInt32(CURLINFO_PROTOCOL))
		{
			switch (*protocol)
			{
				case CURLPROTO_DICT:
				{
					return WebRequestProtocol::DICT;
				}
				case CURLPROTO_FILE:
				{
					return WebRequestProtocol::FILE;
				}
				case CURLPROTO_FTP:
				{
					return WebRequestProtocol::FTP;
				}
				case CURLPROTO_FTPS:
				{
					return WebRequestProtocol::FTPS;
				}
				case CURLPROTO_GOPHER:
				{
					return WebRequestProtocol::GOPHER;
				}
				case CURLPROTO_HTTP:
				{
					return WebRequestProtocol::HTTP;
				}
				case CURLPROTO_HTTPS:
				{
					return WebRequestProtocol::HTTPS;
				}
				case CURLPROTO_IMAP:
				{
					return WebRequestProtocol::IMAP;
				}
				case CURLPROTO_IMAPS:
				{
					return WebRequestProtocol::IMAPS;
				}
				case CURLPROTO_LDAP:
				{
					return WebRequestProtocol::LDAP;
				}
				case CURLPROTO_LDAPS:
				{
					return WebRequestProtocol::LDAPS;
				}
				case CURLPROTO_POP3:
				{
					return WebRequestProtocol::POP3;
				}
				case CURLPROTO_POP3S:
				{
					return WebRequestProtocol::POP3S;
				}
				case CURLPROTO_RTMP:
				{
					return WebRequestProtocol::RTMP;
				}
				case CURLPROTO_RTMPE:
				{
					return WebRequestProtocol::RTMPE;
				}
				case CURLPROTO_RTMPS:
				{
					return WebRequestProtocol::RTMPS;
				}
				case CURLPROTO_RTMPT:
				{
					return WebRequestProtocol::RTMPT;
				}
				case CURLPROTO_RTMPTE:
				{
					return WebRequestProtocol::RTMPTE;
				}
				case CURLPROTO_RTMPTS:
				{
					return WebRequestProtocol::RTMPTS;
				}
				case CURLPROTO_RTSP:
				{
					return WebRequestProtocol::RTSP;
				}
				case CURLPROTO_SCP:
				{
					return WebRequestProtocol::SCP;
				}
				case CURLPROTO_SFTP:
				{
					return WebRequestProtocol::SFTP;
				}
				case CURLPROTO_SMB:
				{
					return WebRequestProtocol::SMB;
				}
				case CURLPROTO_SMBS:
				{
					return WebRequestProtocol::SMBS;
				}
				case CURLPROTO_SMTP:
				{
					return WebRequestProtocol::SMTP;
				}
				case CURLPROTO_SMTPS:
				{
					return WebRequestProtocol::SMTPS;
				}
				case CURLPROTO_TELNET:
				{
					return WebRequestProtocol::TELNET;
				}
				case CURLPROTO_TFTP:
				{
					return WebRequestProtocol::TFTP;
				}
			};
		}
		return WebRequestProtocol::None;
	}
	WebRequestHTTPVersion CURLResponse::GetHTTPVersion() const
	{
		switch (GetRequestHandle().GetOptionUInt32(CURLINFO_HTTP_VERSION).value_or(0))
		{
			case CURL_HTTP_VERSION_1_0:
			{
				return WebRequestHTTPVersion::Version1_0;
			}
			case CURL_HTTP_VERSION_1_1:
			{
				return WebRequestHTTPVersion::Version1_1;
			}
			case CURL_HTTP_VERSION_2_0:
			{
				return WebRequestHTTPVersion::Version2;
			}
			case CURL_HTTP_VERSION_3:
			{
				return WebRequestHTTPVersion::Version3;
			}
		};
		return WebRequestHTTPVersion::None;
	}
	BinarySize CURLResponse::GetContentLength() const
	{
		if (auto value = GetRequestHandle().GetOptionUInt64(CURLINFO_CONTENT_LENGTH_DOWNLOAD_T))
		{
			return BinarySize::FromBytes(*value);
		}
		return {};
	}
	String CURLResponse::GetContentType() const
	{
		return GetRequestHandle().GetOptionString(CURLINFO_CONTENT_TYPE).value_or(NullString);
	}

	String CURLResponse::GetHeader(const String& name) const
	{
		auto it = Utility::Container::FindIf(m_Request.m_ResponseHeaders, [&](const WebRequestHeader& header)
		{
			return header.GetName() == name;
		});
		if (it != m_Request.m_ResponseHeaders.end())
		{
			return it->GetValue();
		}
		return {};
	}
	Enumerator<WebRequestHeader> CURLResponse::EnumHeaders() const
	{
		return Utility::EnumerateIterableContainer<WebRequestHeader>(m_Request.m_ResponseHeaders);
	}
	Enumerator<String> CURLResponse::EnumCookies() const
	{
		if (auto cookesList = static_cast<curl_slist*>(GetRequestHandle().GetOptionPtr(CURLINFO_COOKIELIST).value_or(nullptr)))
		{
			return[handle = make_handle_ptr<::curl_slist_free_all>(cookesList), item = cookesList]() mutable -> std::optional<String>
			{
				auto curentItem = item;
				item = item->next;

				if (curentItem)
				{
					return String::FromUTF8(curentItem->data);
				}
				return {};
			};
		}
		return {};
	}

	String CURLResponse::GetSuggestedFileName() const
	{
		if (String contentDisposition = GetHeader(wxS("Content-Disposition")); !contentDisposition.IsEmpty())
		{
			if (RegEx regEx(wxS("filename=\"(.+)\""), RegExFlag::IgnoreCase); regEx.Matches(contentDisposition))
			{
				return FSPath(regEx.GetMatch(contentDisposition, 1)).GetName();
			}
		}

		if (URI uri = GetURI())
		{
			if (uri.HasPath())
			{
				return FSPath(uri.GetPath()).GetName();
			}
			return uri.GetServer();
		}
		return {};
	}
	IInputStream& CURLResponse::GetStream()
	{
		if (m_Request.m_ReceiveStream)
		{
			switch (m_Request.m_ReceiveStorage)
			{
				case WebRequestStorage::Memory:
				{
					// Our receiving stream must implement 'IMemoryStream' interface since we create it as 'MemoryOutputStream',
					// but check anyway just in case.
					if (auto receiveMemoryStream = m_Request.m_ReceiveStream->QueryInterface<IMemoryStream>())
					{
						const auto& buffer = receiveMemoryStream->GetStreamBuffer();
						m_MemoryStream.emplace(buffer.GetBufferStart(), buffer.GetBufferSize());

						return *m_MemoryStream;
					}
					break;
				}
				case WebRequestStorage::Stream:
				case WebRequestStorage::FileSystem:
				{
					// TODO: Input stream on receiving output stream (using 'IReadableOutputStream' interface).
					break;
				}
			};
		}
		return NullInputStream::Get();
	}
}
