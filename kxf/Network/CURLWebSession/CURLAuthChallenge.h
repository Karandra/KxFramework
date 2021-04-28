#pragma once
#include "Common.h"

namespace kxf
{
	class CURLRequest;
}

namespace kxf
{
	class CURLAuthChallenge final: public IWebAuthChallenge
	{
		private:
			CURLRequest& m_Request;
			WebAuthChallengeSource m_Source = WebAuthChallengeSource::None;

		public:
			CURLAuthChallenge(CURLRequest& request, WebAuthChallengeSource source)
				:m_Request(request), m_Source(source)
			{
			}

		public:
			WebAuthChallengeSource GetSource() const override
			{
				return m_Source;
			}
			void SetCredentials(UserCredentials credentials) override;
	};
}
