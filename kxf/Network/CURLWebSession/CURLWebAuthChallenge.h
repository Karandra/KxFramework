#pragma once
#include "Common.h"

namespace kxf
{
	class CURLWebRequest;
}

namespace kxf
{
	class CURLWebAuthChallenge final: public IWebAuthChallenge
	{
		private:
			CURLWebRequest& m_Request;
			WebAuthChallengeSource m_Source = WebAuthChallengeSource::None;

		public:
			CURLWebAuthChallenge(CURLWebRequest& request, WebAuthChallengeSource source)
				:m_Request(request), m_Source(source)
			{
			}

		public:
			WebAuthChallengeSource GetSource() const override
			{
				return m_Source;
			}
			bool SetCredentials(UserCredentials credentials) override;
	};
}
