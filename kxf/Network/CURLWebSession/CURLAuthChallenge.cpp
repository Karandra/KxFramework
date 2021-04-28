#include "KxfPCH.h"
#include "CURLAuthChallenge.h"
#include "CURLRequest.h"

namespace kxf
{
	void CURLAuthChallenge::SetCredentials(UserCredentials credentials)
	{
		m_Request.OnSetAuthChallengeCredentials(m_Source, std::move(credentials));
	}
}
