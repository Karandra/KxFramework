#include "stdafx.h"
#include "MemoryStream.h"

namespace
{
	constexpr size_t g_BufferSize = kxf::BinarySize::FromKB(64).GetBytes();

	kxf::Win32Error TranslateErrorCode(const wxStreamBase& stream)
	{
		using namespace kxf;

		switch (stream.GetLastError())
		{
			case wxStreamError::wxSTREAM_NO_ERROR:
			{
				return Win32Error::Success();
			}
			case wxStreamError::wxSTREAM_EOF:
			{
				return ERROR_HANDLE_EOF;
			}
			case wxStreamError::wxSTREAM_READ_ERROR:
			{
				return ERROR_READ_FAULT;
			}
			case wxStreamError::wxSTREAM_WRITE_ERROR:
			{
				return ERROR_WRITE_FAULT;
			}
		};
		return Win32Error::Fail();
	}

	bool FlushBuffer(wxStreamBuffer& streamBuffer)
	{
		return streamBuffer.FlushBuffer();
	}
	bool SetBufferAllocationSize(wxStreamBuffer& streamBuffer, kxf::BinarySize allocationSize)
	{
		using namespace kxf;

		BinarySize currentOffset = BinarySize::FromBytes(streamBuffer.GetIntPosition());
		if (currentOffset > allocationSize)
		{
			if (allocationSize)
			{
				streamBuffer.SetIntPosition(allocationSize.GetBytes());
			}
			streamBuffer.Truncate();

			return true;
		}
		return false;
	}
}

namespace kxf
{
	// IStream
	ErrorCode MemoryInputStream::GetLastError() const
	{
		return TranslateErrorCode(m_Stream);
	}

	// IInputStream
	IInputStream& MemoryInputStream::Read(IOutputStream& other)
	{
		uint8_t buffer[g_BufferSize];
		while (true)
		{
			BinarySize read = m_Stream.Read(buffer, std::size(buffer)).LastRead();
			if (!read)
			{
				break;
			}
			else if (other.Write(buffer, read.GetBytes()).LastWrite() != read)
			{
				break;
			}
		}

		return *this;
	}

	// MemoryInputStream
	bool MemoryInputStream::Flush()
	{
		return FlushBuffer(*m_Stream.GetInputStreamBuffer());
	}
	bool MemoryInputStream::SetAllocationSize(BinarySize allocationSize)
	{
		return SetBufferAllocationSize(*m_Stream.GetInputStreamBuffer(), allocationSize);
	}
}

namespace kxf
{
	// IStream
	ErrorCode MemoryOutputStream::GetLastError() const
	{
		return TranslateErrorCode(m_Stream);
	}

	// IOutputStream
	IOutputStream& MemoryOutputStream::Write(IInputStream& other)
	{
		uint8_t buffer[g_BufferSize];
		while (true)
		{
			BinarySize read = other.Read(buffer, std::size(buffer)).LastRead();
			if (!read)
			{
				break;
			}
			else if (m_Stream.Write(buffer, read.GetBytes()).LastWrite() != read.GetBytes())
			{
				break;
			}
		}

		return *this;
	}

	bool MemoryOutputStream::Flush()
	{
		return FlushBuffer(*m_Stream.GetOutputStreamBuffer());
	}
	bool MemoryOutputStream::SetAllocationSize(BinarySize allocationSize)
	{
		return SetBufferAllocationSize(*m_Stream.GetOutputStreamBuffer(), allocationSize);
	}
}
