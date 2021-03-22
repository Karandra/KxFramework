#include "KxfPCH.h"
#include "Common.h"

namespace
{
	constexpr wxLogLevel ToWxLogLevel(kxf::Log::Level level) noexcept
	{
		using namespace kxf::Log;

		switch (level)
		{
			case Level::Info:
			{
				return wxLOG_Info;
			}
			case Level::Error:
			{
				return wxLOG_Error;
			}
			case Level::Trace:
			{
				return wxLOG_Trace;
			}
			case Level::Debug:
			{
				return wxLOG_Debug;
			}
			case Level::Status:
			{
				return wxLOG_Status;
			}
			case Level::Message:
			{
				return wxLOG_Message;
			}
			case Level::Warning:
			{
				return wxLOG_Warning;
			}
			case Level::Progress:
			{
				return wxLOG_Progress;
			}
			case Level::FatalError:
			{
				return wxLOG_FatalError;
			}
		};
		return wxLOG_Max;
	}
}

namespace kxf::Log
{
	bool IsEnabled() noexcept
	{
		return wxLog::IsEnabled();
	}
	bool IsLevelEnabled(Level level) noexcept
	{
		return wxLog::IsLevelEnabled(ToWxLogLevel(level), wxLOG_COMPONENT);
	}
	void Enable(bool enable) noexcept
	{
		wxLog::EnableLogging(enable);
	}

	bool IsVerboseEnabled() noexcept
	{
		return wxLog::GetVerbose();
	}
	void EnableVerbose(bool enable) noexcept
	{
		wxLog::SetVerbose(enable);
	}

	bool IsAssertsEnabled() noexcept
	{
		return wxTheAssertHandler != nullptr;
	}
	void EnableAsserts(bool enable) noexcept
	{
		if (enable)
		{
			wxSetAssertHandler([](const wxString& file, int line, const wxString& func, const wxString& condition, const wxString& message)
			{
				Log::Debug(wxS("Assert failed: [File=%1:%2], [Function=%3], [Condition=%4], [Message=%5]"), file, line, func, condition, message);
			});
		}
		else
		{
			wxSetAssertHandler(nullptr);
		}
	}

	wxLog* GetActiveTarget() noexcept
	{
		return wxLog::GetActiveTarget();
	}
	std::unique_ptr<wxLog> SetActiveTarget(std::unique_ptr<wxLog> logTarget) noexcept
	{
		return std::unique_ptr<wxLog>(wxLog::SetActiveTarget(logTarget.release()));
	}
	std::unique_ptr<wxLog> SetThreadActiveTarget(std::unique_ptr<wxLog> logTarget) noexcept
	{
		return std::unique_ptr<wxLog>(wxLog::SetThreadActiveTarget(logTarget.release()));
	}

	void LogString(Level level, const kxf::String& value)
	{
		const wxLogLevel logWxLevel = ToWxLogLevel(level);
		if (wxLog::IsLevelEnabled(logWxLevel, wxLOG_COMPONENT))
		{
			wxLogger logger(logWxLevel, __FILE__, __LINE__, __WXFUNCTION__, wxLOG_COMPONENT);
			logger.Log(wxS("%s"), value.GetWxString());
		}
	}
}
