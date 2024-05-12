#include "KxfPCH.h"
#include "ScopedLoggerTarget.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include <iostream>

namespace
{
	kxf::String FormatTimestamp(kxf::DateTime timestamp, const kxf::TimeZoneOffset& tzOffset)
	{
		return timestamp.Format("%Y-%m-%d %H-%M-%S.%l", tzOffset);
	}
}

namespace kxf
{
	// IScopedLoggerTarget
	void ScopedLoggerConsoleTarget::Write(LogLevel logLevel, StringView str)
	{
		std::wcout << str << std::endl;
	}
	void ScopedLoggerConsoleTarget::Flush()
	{
		std::wcout.flush();
	}
}

namespace kxf
{
	ScopedLoggerFileTarget::ScopedLoggerFileTarget(ScopedLoggerTLS& tls, const FSPath& directory)
	{
		auto timestamp = tls.GetTimestamp();
		auto directoryTimestamp = tls.GetGlobalContext().GetUnknownThreadContext().GetTimestamp();
		auto tzOffset = tls.GetGlobalContext().GetTimeOffset();

		String path;
		path.Format("[{}] {}-{}\\", FormatTimestamp(directoryTimestamp, tzOffset), tls.GetProcess().GetID(), tls.GetProcess().GetExecutablePath().GetName());
		if (!tls.IsUnknown())
		{
			path.Format("[{}] {}.log", FormatTimestamp(timestamp, tzOffset), tls.GetThread().GetID());
		}
		else
		{
			path.Format("[{}] UnknownContext.log", FormatTimestamp(timestamp, tzOffset));
		}

		NativeFileSystem fs(directory ? directory : NativeFileSystem::GetExecutingModuleWorkingDirectory());
		m_Stream = fs.OpenToWrite(path, IOStreamDisposition::CreateAlways, IOStreamShare::Read, FSActionFlag::CreateDirectoryTree|FSActionFlag::Recursive);
	}

	// IScopedLoggerTarget
	void ScopedLoggerFileTarget::Write(LogLevel logLevel, StringView str)
	{
		IO::OutputStreamWriter writer(*m_Stream);
		writer.WriteStringUTF8(String(str).Append('\n', 1));

		if (++m_WriteCount >= 16)
		{
			m_WriteCount = 0;
			Flush();

			return;
		}

		switch (logLevel)
		{
			case LogLevel::FlowControl:
			case LogLevel::Critical:
			case LogLevel::Warning:
			case LogLevel::Error:
			{
				Flush();
				break;
			}
		};
	}
	void ScopedLoggerFileTarget::Flush()
	{
		m_Stream->Flush();
	}
}
