#include "KxfPCH.h"
#include "ScopedLogger.h"
#include "ScopedLoggerTarget.h"
#include "Private/WxOverride.h"
#include "kxf/System/DynamicLibrary.h"

namespace
{
	kxf::String FormatTimestamp(kxf::DateTime timestamp, const kxf::TimeZoneOffset& tzOffset)
	{
		return timestamp.Format("%Y-%m-%d/%T", tzOffset).Format(".{:03}", timestamp.GetMillisecond(tzOffset));
	}
	const char* FormatLogLevel(kxf::LogLevel logLevel)
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
		return "<UNKNOWN>";
	}
}

namespace kxf
{
	ScopedLoggerGlobalContext& ScopedLoggerGlobalContext::GetInstance() noexcept
	{
		static ScopedLoggerGlobalContext context;
		return context;
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

		Log::WxOverride::Install();
	}
	void ScopedLoggerGlobalContext::Destroy()
	{
		m_TLSIndex.Uninitialize();
		m_UnknownContext = {};
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
	std::shared_ptr<IScopedLoggerTarget> ScopedLoggerTLS::CreateLogTarget()
	{
		if (auto context = m_GlobalContext.GetUserContext())
		{
			return context->CreateLogTarget(*this);
		}
		return std::make_shared<ConsoleScopedLoggerTarget>(*this);
	}

	void ScopedLoggerTLS::Flush()
	{
		if (m_LogTarget)
		{
			m_LogTarget->Flush();
		}
	}
	void ScopedLoggerTLS::Write(LogLevel logLevel, DateTime timestamp, StringView str)
	{
		if (!str.empty() && m_GlobalContext.CanLogLevel(logLevel))
		{
			String buffer;

			// Log time
			buffer.Format("[{}]", FormatTimestamp(timestamp, m_GlobalContext.GetTimeOffset()));

			// Log location
			if (IsUnknown())
			{
				buffer.Format("[{}/<unk>:{:06}]", m_Process.GetID(), SystemThread::GetCurrentThread().GetID());
			}
			else
			{
				buffer.Format("[{}/{}]", m_Process.GetID(), m_Thread.GetID());
			}

			// Log message level
			buffer.Format("[{:11}]", FormatLogLevel(logLevel));

			// Add indents
			buffer.Append(' ', m_ScopeLevel * 4);

			// Log the actual message string
			buffer += ' ';
			buffer += str;

			// Write the whole thing out
			m_LogTarget->Write(logLevel, buffer.xc_view());
		}
	}
}

namespace kxf
{
	void ScopedLoggerUnknownTLS::Initialize()
	{
		m_Thread = {};
		m_Process = SystemProcess::GetCurrentProcess();
		m_LogTarget = CreateLogTarget();

		m_Scope.emplace(*this, "<unknown context>()");
		m_Scope->SetSuccess();

		LogOpenClose(true);
	}
}

namespace kxf
{
	void ScopedMessageLogger::Write()
	{
		if (m_Scope)
		{
			m_Scope->Write(m_LogLevel, m_TimeStamp, m_Buffer.xc_view());
		}
	}

	void ScopedLogger::OnEnter(StringView parameters)
	{
		{
			auto logger = FlowControl();

			logger.Log("Enter: ", m_Function);
			if (!parameters.empty())
			{
				if (m_Function.Contains('('))
				{
					logger.Format(" /. {{{}}}", parameters);
				}
				else
				{
					logger.Format(" ({})", parameters);
				}
			}
		}
		m_ScopeTLS.OnScopeEnter(*this);
	}
	void ScopedLogger::OnLeave()
	{
		m_ScopeTLS.OnScopeLeave(*this);

		auto funcName = m_Function.xc_view();
		if (auto pos = funcName.find('('); pos != funcName.npos)
		{
			funcName.remove_suffix(funcName.length() - pos);
		}

		{
			auto logger = FlowControl();

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