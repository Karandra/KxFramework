#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/Format.h"
class wxLog;

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
	KX_API bool IsEnabled() noexcept;
	KX_API bool IsLevelEnabled(Level level) noexcept;
	KX_API void Enable(bool enable = true) noexcept;

	KX_API bool IsVerboseEnabled() noexcept;
	KX_API void EnableVerbose(bool enable = true) noexcept;

	KX_API bool IsAssertsEnabled() noexcept;
	KX_API void EnableAsserts(bool enable = true) noexcept;

	KX_API wxLog* GetActiveTarget() noexcept;
	KX_API std::unique_ptr<wxLog> SetActiveTarget(std::unique_ptr<wxLog> logTarget) noexcept;
	KX_API std::unique_ptr<wxLog> SetThreadActiveTarget(std::unique_ptr<wxLog> logTarget) noexcept;

	KX_API void LogString(Level level, const String& value);
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
				LogString(level, Format(format, std::forward<Args>(arg)...));
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
