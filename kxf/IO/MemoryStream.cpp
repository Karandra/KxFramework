#include "stdafx.h"
#include "MemoryStream.h"

namespace
{
	bool FlushBuffer(wxStreamBuffer& streamBuffer)
	{
		return streamBuffer.IsFlushable() && streamBuffer.FlushBuffer();
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
	MemoryInputStream::MemoryInputStream(const MemoryOutputStream& stream)
		:InputStreamWrapper(m_Stream), m_Stream(stream.AsWxStream())
	{
	}
	MemoryInputStream::MemoryInputStream(IInputStream& stream, BinarySize size)
		:InputStreamWrapper(m_Stream), m_Stream(nullptr, 0)
	{
		size = size ? size : stream.GetSize();
		if (auto buffer = m_Stream.GetInputStreamBuffer())
		{
			buffer->SetBufferIO(size.GetBytes());
			buffer->SetIntPosition(0);
			buffer->Fixed(true);
			if (!stream.ReadAll(buffer->GetBufferStart(), size.GetBytes()))
			{
				buffer->ResetBuffer();
			}
		}
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
	// IOutputStream
	bool MemoryOutputStream::Flush()
	{
		return FlushBuffer(*m_Stream.GetOutputStreamBuffer());
	}
	bool MemoryOutputStream::SetAllocationSize(BinarySize allocationSize)
	{
		return SetBufferAllocationSize(*m_Stream.GetOutputStreamBuffer(), allocationSize);
	}

	// MemoryOutputStream
	size_t MemoryOutputStream::CopyTo(void* buffer, size_t size) const
	{
		const size_t effectiveSize = std::min(size, m_Stream.GetSize());
		std::memcpy(buffer, m_Stream.GetOutputStreamBuffer()->GetBufferStart(), effectiveSize);

		return effectiveSize;
	}
}
