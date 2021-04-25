#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class UserCredentials;

	enum class WebAuthChallengeSource
	{
		None = -1,

		Server,
		Proxy
	};
}

namespace kxf
{
	class IWebAuthChallenge: public RTTI::Interface<IWebAuthChallenge>
	{
		KxRTTI_DeclareIID(IWebAuthChallenge, {0xb2231898, 0xd519, 0x4a61, {0xbc, 0x36, 0xfc, 0x4, 0x5d, 0x9a, 0x7c, 0x13}});

		public:
			virtual WebAuthChallengeSource GetSource() const = 0;
			virtual void SetCredentials(const UserCredentials& credentials) = 0;
	};
}

namespace kxf
{
	class NullWebAuthChallenge final: public IWebAuthChallenge
	{
		public:
			static IWebAuthChallenge& Get();

		public:
			WebAuthChallengeSource GetSource() const override
			{
				return WebAuthChallengeSource::None;
			}
			void SetCredentials(const UserCredentials& credentials) override
			{
			}
	};
}
