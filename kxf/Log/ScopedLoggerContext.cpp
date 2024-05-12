#include "KxfPCH.h"
#include "ScopedLoggerContext.h"
#include "ScopedLoggerTarget.h"

namespace kxf
{
	// IScopedLoggerContext
	std::shared_ptr<IScopedLoggerTarget> ScopedLoggerFileContext::CreateLogTarget(ScopedLoggerTLS& tls)
	{
		return std::make_shared<ScopedLoggerFileTarget>(tls, m_LogDirectory);
	}
}

namespace kxf
{
	// IScopedLoggerContext
	std::shared_ptr<IScopedLoggerTarget> ScopedLoggerAggregateContext::CreateLogTarget(ScopedLoggerTLS& tls)
	{
		return m_AggregateTarget;
	}
}
