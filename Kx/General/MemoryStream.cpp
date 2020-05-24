#include "stdafx.h"
#include "MemoryStream.h"

namespace
{
	bool FlushBuffer(wxStreamBuffer& streamBuffer)
	{
		return streamBuffer.FlushBuffer();
	}
	bool SetBufferAllocationSize(wxStreamBuffer& streamBuffer, kxf::BinarySize offset)
	{
		using namespace kxf;

		BinarySize currentPos = BinarySize::FromBytes(streamBuffer.GetIntPosition());
		if (currentPos > offset)
		{
			if (offset)
			{
				streamBuffer.SetIntPosition(offset.GetBytes());
			}
			streamBuffer.Truncate();

			return true;
		}
		return false;
	}
}

namespace kxf
{
	bool MemoryInputStream::Flush()
	{
		return FlushBuffer(*GetInputStreamBuffer());
	}
	bool MemoryInputStream::SetAllocationSize(BinarySize offset)
	{
		return SetBufferAllocationSize(*GetInputStreamBuffer(), offset);
	}
}

namespace kxf
{
	bool MemoryOutputStream::Flush()
	{
		return FlushBuffer(*GetOutputStreamBuffer());
	}
	bool MemoryOutputStream::SetAllocationSize(BinarySize offset)
	{
		return SetBufferAllocationSize(*GetOutputStreamBuffer(), offset);
	}
}
