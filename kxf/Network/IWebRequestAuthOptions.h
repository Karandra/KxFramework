#pragma once
#include "Common.h"
#include "IWebRequestOptions.h"

namespace kxf
{
	enum class WebRequestAuthMethod: uint32_t
	{
		None = 0,

		Basic = 1 << 0,
		NTLM = 1 << 1,
		Digest = 1 << 2,
		Bearer = 1 << 3,
		Negotiate = 1 << 4
	};
}

namespace kxf
{
	class IWebRequestAuthOptions: public RTTI::Interface<IWebRequestAuthOptions>
	{
		KxRTTI_DeclareIID(IWebRequestAuthOptions, {0x268dd52, 0x8f20, 0x40c0, {0x81, 0x5f, 0xaf, 0x36, 0x93, 0x8, 0x61, 0xac}});

		public:
			virtual bool SetAuthMethod(WebRequestAuthMethod method) = 0;
			virtual bool SetAuthMethods(FlagSet<WebRequestAuthMethod> methods) = 0;

			virtual bool SetUserName(const String& userName) = 0;
			virtual bool SetUserPassword(const String& userPassword) = 0;
	};
}
