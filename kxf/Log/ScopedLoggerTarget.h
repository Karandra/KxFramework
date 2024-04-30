#pragma once
#include "Common.h"
#include "ScopedLogger.h"
#include "kxf/IO/IStream.h"
#include "kxf/FileSystem/FSPath.h"

namespace kxf
{
	class ConsoleScopedLoggerTarget: public IScopedLoggerTarget
	{
		public:
			ConsoleScopedLoggerTarget(ScopedLoggerTLS& tls)
			{
			}

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override;
			void Flush() override;
	};

	class FileScopedLoggerTarget: public IScopedLoggerTarget
	{
		private:
			std::unique_ptr<IOutputStream> m_Stream;
			size_t m_WriteCount = 0;

		public:
			FileScopedLoggerTarget(ScopedLoggerTLS& tls, const FSPath& directory);

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override;
			void Flush() override;
	};

	class AggregateScopedLoggerTarget: public IScopedLoggerTarget
	{
		public:
			std::vector<std::shared_ptr<IScopedLoggerTarget>> m_LogTargets;

		public:
			// IScopedLoggerTarget
			void Write(LogLevel logLevel, StringView str) override
			{
				for (auto& ptr: m_LogTargets)
				{
					ptr->Write(logLevel, str);
				}
			}
			void Flush() override
			{
				for (auto& ptr: m_LogTargets)
				{
					ptr->Flush();
				}
			}

			// AggregateScopedLoggerTarget
			void PushTarget(std::shared_ptr<IScopedLoggerTarget> ptr)
			{
				m_LogTargets.emplace_back(std::move(ptr));
			}
			std::shared_ptr<IScopedLoggerTarget> PopTarget()
			{
				if (!m_LogTargets.empty())
				{
					auto ptr = std::move(m_LogTargets.back());
					m_LogTargets.pop_back();

					return ptr;
				}
				return nullptr;
			}
	};

	class FileScopedLoggerContext: public IScopedLoggerContext
	{
		private:
			FSPath m_LogDirectory;

		public:
			FileScopedLoggerContext(FSPath logDirectory)
				:m_LogDirectory(std::move(logDirectory))
			{
			}

		public:
			// IScopedLoggerContext
			std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) override
			{
				return std::make_shared<FileScopedLoggerTarget>(tls, m_LogDirectory);
			}
	};
}
