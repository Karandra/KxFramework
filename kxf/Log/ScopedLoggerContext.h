#pragma once
#include "Common.h"
#include "ScopedLogger.h"
#include "kxf/FileSystem/FSPath.h"

namespace kxf
{
	class ScopedLoggerFileTarget;
	class ScopedLoggerFileContext: public IScopedLoggerContext
	{
		private:
			FSPath m_LogDirectory;

		public:
			ScopedLoggerFileContext(FSPath logDirectory)
				:m_LogDirectory(std::move(logDirectory))
			{
			}

		public:
			// IScopedLoggerContext
			std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) override;
	};
}

namespace kxf
{
	class ScopedLoggerAggregateTarget;
	class ScopedLoggerAggregateContext: public IScopedLoggerContext
	{
		private:
			std::shared_ptr<ScopedLoggerAggregateTarget> m_AggregateTarget;

		public:
			ScopedLoggerAggregateContext(std::shared_ptr<ScopedLoggerAggregateTarget> aggregateTarget)
				:m_AggregateTarget(std::move(aggregateTarget))
			{
			}

		public:
			// IScopedLoggerContext
			std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) override;
	};
}
