#include "KxfPCH.h"
#include "ScopedLoggerTarget.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include <iostream>

namespace kxf
{
	void ConsoleScopedLoggerTarget::Write(LogLevel logLevel, StringView str)
	{
		std::wcout << str << std::endl;
	}
	void ConsoleScopedLoggerTarget::Flush()
	{
		std::wcout.flush();
	}
}

namespace kxf
{
	FileScopedLoggerTarget::FileScopedLoggerTarget(ScopedLoggerTLS& tls, const FSPath& directory)
	{
		String path;
		path.Format("{}-{}\\", tls.GetProcess().GetID(), tls.GetProcess().GetExecutablePath().GetName());
		if (auto thread = tls.GetThread(); !thread.IsNull())
		{
			path.Format("{}.log", thread.GetID());
		}
		else
		{
			path += "Unknown.log";
		}

		NativeFileSystem fs(directory);
		m_Stream = fs.OpenToWrite(path, IOStreamDisposition::CreateAlways, IOStreamShare::Read, FSActionFlag::CreateDirectoryTree);
	}

	void FileScopedLoggerTarget::Write(LogLevel logLevel, StringView str)
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
	void FileScopedLoggerTarget::Flush()
	{
		m_Stream->Flush();
	}
}
