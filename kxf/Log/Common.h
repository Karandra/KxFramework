#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/StringFormatter.h"

namespace kxf::Log
{
	enum class Level
	{
		Info,
		Error,
		Trace,
		Debug,
		Status,
		Message,
		Warning,
		Progress,
		FatalError,
	};
}

namespace kxf::Log
{
	bool IsEnabled() noexcept;
	bool IsLevelEnabled(Level level) noexcept;
	void Enable(bool enable = true) noexcept;

	bool IsVerboseEnabled() noexcept;
	void EnableVerbose(bool enable = true) noexcept;

	bool IsAssertsEnabled() noexcept;
	void EnableAsserts(bool enable = true) noexcept;

	wxLog* GetActiveTarget() noexcept;
	std::unique_ptr<wxLog> SetActiveTarget(std::unique_ptr<wxLog> logTarget) noexcept;
	std::unique_ptr<wxLog> SetThreadActiveTarget(std::unique_ptr<wxLog> logTarget) noexcept;

	void LogString(Level level, const String& value);
}

namespace kxf::Log
{
	namespace Private
	{
		inline void LogIfEnabled(Level level, const String& format)
		{
			if (IsLevelEnabled(level))
			{
				LogString(level, format);
			}
		}

		template<class... Args>
		void LogIfEnabled(Level level, const String& format, Args&&... arg)
		{
			if (IsLevelEnabled(level))
			{
				LogString(level, String::Format(format, std::forward<Args>(arg)...));
			}
		}
	}

	template<class... Args>
	void Info(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Info, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void Error(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Error, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void Trace(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Trace, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void Debug(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Debug, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void Status(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Status, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void Message(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Message, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void Warning(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Warning, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void Progress(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::Progress, format, std::forward<Args>(arg)...);
	}

	template<class... Args>
	void FatalError(const kxf::String& format, Args&&... arg)
	{
		Private::LogIfEnabled(Level::FatalError, format, std::forward<Args>(arg)...);
	}
}
