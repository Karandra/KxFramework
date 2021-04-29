#include "KxfPCH.h"
#include "CURLAuthChallenge.h"
#include "CURLRequest.h"

namespace kxf
{
	bool CURLAuthChallenge::SetCredentials(UserCredentials credentials)
	{
		return m_Request.OnSetAuthChallengeCredentials(m_Source, std::move(credentials));
	}
}
