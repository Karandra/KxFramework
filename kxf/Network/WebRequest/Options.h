#pragma once
#include "Common.h"

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
