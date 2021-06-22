#include "KxfPCH.h"
#include "WebRequest.h"
#include "WSPPWebSession/WSPPWebSession.h"
#include "CURLWebSession/CURLWebSession.h"

namespace kxf::Network
{
	std::unique_ptr<IWebSession> CreateWebSession(const URI& uri, optional_ptr<IThreadPool> threadPool)
	{
		if (uri)
		{
			std::unique_ptr<IWebSession> session;

			const auto scheme = uri.GetScheme();
			if (scheme.IsSameAs("ws", StringActionFlag::IgnoreCase) || scheme.IsSameAs("wss", StringActionFlag::IgnoreCase))
			{
				session = std::make_unique<WSPPWebSession>(std::move(threadPool));
			}
			else
			{
				session = std::make_unique<CURLWebSession>(std::move(threadPool));
			}

			if (session)
			{
				session->SetBaseURI(uri);
			}
			return session;
		}
		return nullptr;
	}
}
