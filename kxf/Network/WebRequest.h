#pragma once
#include "WebRequest/IWebSession.h"
#include "WebRequest/IWebRequest.h"
#include "WebRequest/IWebResponse.h"
#include "WebRequest/IWebAuthChallenge.h"
#include "WebRequest/WebRequestEvent.h"
#include "WebRequest/WebRequestHeader.h"

#include "WebRequest/IWebRequestOptions.h"
#include "WebRequest/IWebRequestAuthOptions.h"
#include "WebRequest/IWebRequestProxyOptions.h"
#include "WebRequest/IWebRequestSecurityOptions.h"

#include "kxf/Core/OptionalPtr.h"

namespace kxf
{
	class IThreadPool;
}

namespace kxf::Network
{
	KX_API std::unique_ptr<IWebSession> CreateWebSession(const URI& uri, optional_ptr<IThreadPool> threadPool = nullptr);
}
