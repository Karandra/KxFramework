#include "KxfPCH.h"
#include "ScopedLogger.h"
#include "ScopedLoggerTarget.h"
#include "Private/WxOverride.h"
#include "kxf/System/DynamicLibrary.h"

namespace
{
	kxf::String FormatTimestamp(kxf::DateTime timestamp, const kxf::TimeZoneOffset& tzOffset)
	{
		return timestamp.Format("%Y-%m-%d/%H:%M:%S.%l", tzOffset);
	}
	std::string_view FormatLogLevel(kxf::LogLevel logLevel)
	{
		using namespace kxf;

		switch (logLevel)
		{
			case LogLevel::FlowControl:
			{
				return "FlowControl";
			}
			case LogLevel::Trace:
			{
				return "Trace";
			}
			case LogLevel::Debug:
			{
				return "Debug";
			}
			case LogLevel::Unknown:
			case LogLevel::Information:
			{
				return "Information";
			}
			case LogLevel::Warning:
			{
				return "Warning";
			}
			case LogLevel::Error:
			{
				return "Error";
			}
			case LogLevel::Critical:
			{
				return "Critical";
			}
		};
		return {};
	}
}

namespace kxf
{
	String ToString(LogLevel value)
	{
		return FormatLogLevel(value);
	}

	ScopedLoggerGlobalContext& ScopedLoggerGlobalContext::Initialize(std::shared_ptr<IScopedLoggerContext> userContext)
	{
		static ScopedLoggerGlobalContext context(std::move(userContext));
		return context;
	}
	ScopedLoggerGlobalContext& ScopedLoggerGlobalContext::GetInstance() noexcept
	{
		return Initialize(nullptr);
	}

	void ScopedLoggerGlobalContext::Initialize()
	{
		m_TLSIndex.Initialize([](void* ptr)
		{
			if (auto tls = Utility::AssumeUniquePtr(static_cast<ScopedLoggerTLS*>(ptr)))
			{
				tls->OnTermination();
			}
		});
		if (!m_TLSIndex)
		{
			throw std::runtime_error(__FUNCTION__ ": Failed to initialize thread local slot");
		}

		m_TimeOffset = TimeZone::Local;
		m_UnknownContext = std::make_unique<ScopedLoggerUnknownTLS>(*this);
		m_UnknownContext->Initialize();

		Log::WxOverride::Install();
	}
	void ScopedLoggerGlobalContext::Destroy()
	{
		if (m_UnknownContext)
		{
			m_UnknownContext->Destroy();
			m_UnknownContext = {};
		}
		m_TLSIndex.Uninitialize();
	}

	ScopedLoggerTLS* ScopedLoggerGlobalContext::QueryThreadContext() const noexcept
	{
		if (void* ptr = m_TLSIndex.GetValue())
		{
			return static_cast<ScopedLoggerTLS*>(ptr);
		}
		return nullptr;
	}
	ScopedLoggerTLS& ScopedLoggerGlobalContext::GetThreadContext()
	{
		if (auto ptr = QueryThreadContext())
		{
			return *ptr;
		}
		else
		{
			auto tls = std::make_unique<ScopedLoggerTLS>(*this);
			tls->Initialize();

			ptr = tls.release();
			m_TLSIndex.SetValue(ptr);
			return *ptr;
		}
	}
	bool ScopedLoggerGlobalContext::CanLogLevel(LogLevel logLevel) const noexcept
	{
		if (logLevel == LogLevel::Disabled)
		{
			return false;
		}
		else if (logLevel == LogLevel::FlowControl)
		{
			return true;
		}
		else
		{
			if (logLevel == LogLevel::Unknown)
			{
				logLevel = LogLevel::Information;
			}
			return ToInt(logLevel) <= ToInt(m_LogLevel.load());
		}
	}
}

namespace kxf
{
	void ScopedLoggerTLS::Initialize()
	{
		m_Thread = SystemThread::GetCurrentThread();
		m_Process = SystemProcess::GetCurrentProcess();
		m_TimeStamp = DateTime::Now();
		m_LogTarget = CreateLogTarget();

		LogOpenClose(true);
	}
	void ScopedLoggerTLS::Destroy()
	{
		if (m_LogTarget)
		{
			LogOpenClose(false);

			m_LogTarget->Flush();
			m_LogTarget = nullptr;
		}
	}

	void ScopedLoggerTLS::LogOpenClose(bool open)
	{
		if (!m_LogTarget)
		{
			return;
		}

		if (open)
		{
			ScopedMessageLogger message(LogLevel::FlowControl);
			message.SetSeparator('/');

			if (IsUnknown())
			{
				message << "Log opened for " << m_Process.GetExecutablePath().GetName();
				message.Sep();
			}
			else
			{
				message << "Log opened for thread " << m_Thread.GetID();
				message.Sep();

				if (auto description = m_Thread.GetDescription(); !description.IsEmpty())
				{
					message << description;
					message.Sep();
				}
			}
			message << DynamicLibrary::GetCurrentModule().GetFilePath().GetName();

			message.Write(*this);
		}
		else
		{
			ScopedMessageLogger message(LogLevel::FlowControl);
			message.SetSeparator('/');

			if (IsUnknown())
			{
				message << "Log closed for " << m_Process.GetExecutablePath().GetName();
				message.Sep();
			}
			else
			{
				message << "Log closed for thread " << m_Thread.GetID();
				message.Sep();
			}
			message << DynamicLibrary::GetCurrentModule().GetFilePath().GetName();

			message.Write(*this);
		}
	}
	String ScopedLoggerTLS::FormatRecord(LogLevel logLevel, DateTime timestamp, StringView message) const
	{
		String buffer;
		buffer.reserve(255);

		// Log time
		buffer.Format("[{}]", FormatTimestamp(timestamp, m_GlobalContext.GetTimeOffset()));

		// Log location
		if (IsUnknown())
		{
			buffer.Format("[{}/<unk>:{:05}]", m_Process.GetID(), SystemThread::GetCurrentThread().GetID());
		}
		else
		{
			buffer.Format("[{:05}/{:05}]", m_Process.GetID(), m_Thread.GetID());
		}

		// Log message level
		buffer.Format("[{:11}]", FormatLogLevel(logLevel));

		// Add indents
		buffer.Append(' ', m_ScopeLevel * 4);

		// Log the actual message string
		buffer += ' ';
		buffer += message;

		return buffer;
	}
	std::shared_ptr<IScopedLoggerTarget> ScopedLoggerTLS::CreateLogTarget()
	{
		if (auto context = m_GlobalContext.GetUserContext())
		{
			return context->CreateLogTarget(*this);
		}
		return std::make_shared<ScopedLoggerConsoleTarget>(*this);
	}

	void ScopedLoggerTLS::Flush()
	{
		if (m_LogTarget)
		{
			m_LogTarget->Flush();
		}
	}
	void ScopedLoggerTLS::Write(LogLevel logLevel, DateTime timestamp, StringView message)
	{
		if (!message.empty() && m_GlobalContext.CanLogLevel(logLevel))
		{
			String formatted = m_LogTarget->FormatRecord(*this, logLevel, timestamp, message);
			if (formatted.IsEmpty())
			{
				formatted = FormatRecord(logLevel, timestamp, message);
			}

			m_LogTarget->Write(logLevel, formatted.xc_view());
		}
	}
}

namespace kxf
{
	void ScopedLogger::OnEnter(StringView serializedParameters)
	{
		if (CanEnterLeave())
		{
			if (auto logger = FlowControl(); true)
			{
				logger.Log("Enter: ", m_Function);
				if (!serializedParameters.empty())
				{
					logger.Format(" ({})", serializedParameters);
				}
			}
			m_ScopeTLS.OnScopeEnter(*this);
		}
	}
	void ScopedLogger::OnLeave()
	{
		if (CanEnterLeave())
		{
			m_ScopeTLS.OnScopeLeave(*this);

			auto funcName = m_Function.xc_view();
			if (auto pos = funcName.find('('); pos != funcName.npos)
			{
				funcName.remove_suffix(funcName.length() - pos);
			}

			if (auto logger = FlowControl(); true)
			{
				logger.Log("Leave: ");
				if (!m_IsSuccess)
				{
					logger << "<FAILED> ";
				}
				logger << funcName;
				if (!m_IsVoid && !m_ReturnValue.empty())
				{
					logger << " -> " << m_ReturnValue;
				}
			}
		}
	}
}

namespace kxf
{
	void ScopedLoggerUnknown::Initialize()
	{
		m_Function = "<unknown context>()";
		m_IsVoid = true;
		m_IsSuccess = true;
	}
	void ScopedLoggerUnknown::Destroy()
	{
		m_Function.clear();
	}

	void ScopedLoggerUnknownTLS::Initialize()
	{
		m_Thread = {};
		m_Process = SystemProcess::GetCurrentProcess();
		m_TimeStamp = DateTime::Now();
		m_LogTarget = CreateLogTarget();
		m_Scope.Initialize();

		LogOpenClose(true);
	}
	void ScopedLoggerUnknownTLS::Destroy()
	{
		m_Scope.Destroy();
		ScopedLoggerTLS::Destroy();
	}
}

namespace kxf
{
	void ScopedMessageLogger::Write()
	{
		if (m_Scope)
		{
			m_Scope->GetTLS().Write(m_LogLevel, m_TimeStamp, m_Buffer.xc_view());
		}
	}
}
