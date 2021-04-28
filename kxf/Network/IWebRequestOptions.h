#pragma once
#include "Common.h"
#include "kxf/IO/Common.h"

// Undef these symbols because Windows SDK defines some of them
#undef OPTIONS
#undef GET
#undef HEAD
#undef POST
#undef PUT
#undef DELETE
#undef TRACE
#undef CONNECT

namespace kxf
{
	enum class WebRequestOption2
	{
		Disabled = 0,
		Enabled = 1
	};
	enum class WebRequestOption3
	{
		Diabled,
		Enabled,
		Required
	};
	enum class WebRequestMethod
	{
		None = -1,

		OPTIONS,
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		TRACE,
		CONNECT,
		PATCH
	};
	enum class WebRequestSSLVersion
	{
		Default = -1,

		SSLv2,
		SSLv3,

		TLSv1_x,
		TLSv1_0,
		TLSv1_1,
		TLSv1_2,
		TLSv1_3
	};

	enum class WebRequestAuthMethod: uint32_t
	{
		None = 0,

		Basic = 1 << 0,
		NTLM = 1 << 1,
		Digest = 1 << 2,
		Bearer = 1 << 3,
		Negotiate = 1 << 4
	};
	KxFlagSet_Declare(WebRequestAuthMethod);

	enum class WebRequestProtocol: uint64_t
	{
		None = 0,
		Everything = std::numeric_limits<uint64_t>::max(),

		DICT = 1_zu << 0,
		FILE = 1_zu << 1,
		FTP = 1_zu << 2,
		FTPS = 1_zu << 3,
		GOPHER = 1_zu << 4,
		HTTP = 1_zu << 5,
		HTTPS = 1_zu << 6,
		IMAP = 1_zu << 7,
		IMAPS = 1_zu << 8,
		LDAP = 1_zu << 9,
		LDAPS = 1_zu << 10,
		POP3 = 1_zu << 11,
		POP3S = 1_zu << 12,
		RTMP = 1_zu << 13,
		RTMPE = 1_zu << 14,
		RTMPS = 1_zu << 15,
		RTMPT = 1_zu << 16,
		RTMPTE = 1_zu << 17,
		RTMPTS = 1_zu << 18,
		RTSP = 1_zu << 19,
		SCP = 1_zu << 20,
		SFTP = 1_zu << 21,
		SMB = 1_zu << 22,
		SMBS = 1_zu << 23,
		SMTP = 1_zu << 24,
		SMTPS = 1_zu << 25,
		TELNET = 1_zu << 26,
		TFTP = 1_zu << 27
	};
	KxFlagSet_Declare(WebRequestProtocol);
}

namespace kxf
{
	class IWebRequestOptions: public RTTI::Interface<IWebRequestOptions>
	{
		KxRTTI_DeclareIID(IWebRequestOptions, {0x6a4c956d, 0x2842, 0x4a2f, {0xaa, 0x30, 0x31, 0x16, 0xb1, 0x58, 0xe9, 0x37}});

		public:
			virtual bool SetURI(const URI& uri) = 0;
			virtual bool SetPort(uint16_t port) = 0;
			virtual bool SetMethod(const String& method) = 0;
			virtual bool SetDefaultProtocol(const String& protocol) = 0;
			virtual bool SetAllowedProtocols(FlagSet<WebRequestProtocol> protocols) = 0;

			virtual bool SetServiceName(const String& name) = 0;
			virtual bool SetAllowRedirection(WebRequestOption2 option) = 0;
			virtual bool SetRedirectionProtocols(FlagSet<WebRequestProtocol> protocols) = 0;
			virtual bool SetResumeOffset(StreamOffset offset) = 0;

			virtual bool SetRequestTimeout(const TimeSpan& timeout) = 0;
			virtual bool SetConnectionTimeout(const TimeSpan& timeout) = 0;

			virtual bool SetMaxSendRate(const TransferRate& rate) = 0;
			virtual bool SetMaxReceiveRate(const TransferRate& rate) = 0;

			virtual bool SetKeepAlive(WebRequestOption2 option) = 0;
			virtual bool SetKeepAliveIdle(const TimeSpan& interval) = 0;
			virtual bool SetKeepAliveInterval(const TimeSpan& interval) = 0;

		public:
			bool SetMethod(WebRequestMethod method);
	};
}
