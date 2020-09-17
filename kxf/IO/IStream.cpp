#include "stdafx.h"
#include "IStream.h"

namespace
{
	constexpr size_t g_BufferSize = kxf::BinarySize::FromKB(64).ToBytes();
}

namespace kxf
{
	IInputStream& IInputStream::Read(IOutputStream& other)
	{
		BinarySize readTotal = 0;
		uint8_t buffer[g_BufferSize];

		while (true)
		{
			BinarySize read = Read(buffer, std::size(buffer)).LastRead();
			if (!read)
			{
				break;
			}
			else if (other.Write(buffer, read.ToBytes()).LastWrite() != read)
			{
				break;
			}

			readTotal += read;
		}
		SetLastRead(readTotal);

		return *this;
	}
	bool IInputStream::ReadAll(void* buffer, size_t size)
	{
		BinarySize readTotal = 0;
		size_t bufferOffset = 0;

		while (true)
		{
			const BinarySize lastRead = Read(static_cast<uint8_t*>(buffer) + bufferOffset, size - bufferOffset).LastRead();

			// There is no point in continuing looping if we can't read anything at all
			if (!lastRead)
			{
				break;
			}
			readTotal += lastRead;

			// Or if an error occurred on the stream
			if (GetLastError().IsFail())
			{
				break;
			}

			// Return successfully if we read exactly the requested number of bytes (normally the ">" case
			// should never occur and so we could use "==" test, but be safe and avoid overflowing size even
			// in case of bugs in 'LastRead').
			if (lastRead >= size)
			{
				size = 0;
				break;
			}

			// Advance the buffer before trying to read the rest of data
			bufferOffset += lastRead.ToBytes();
		}
		SetLastRead(readTotal);

		return size == bufferOffset;
	}
}

namespace kxf
{
	IOutputStream& IOutputStream::Write(IInputStream& other)
	{
		other.Read(*this);
		return *this;
	}
	bool IOutputStream::WriteAll(const void* buffer, size_t size)
	{
		// This function mirrors implementation of 'IInputStream::ReadAll', look there for more comments.

		BinarySize writtenTotal = 0;
		size_t bufferOffset = 0;

		while (true)
		{
			const BinarySize lastWrite = Write(buffer, size).LastWrite();
			if (!lastWrite)
			{
				break;
			}
			writtenTotal += lastWrite;

			if (GetLastError().IsFail())
			{
				break;
			}
			if (lastWrite >= size)
			{
				size = 0;
				break;
			}

			bufferOffset += lastWrite.ToBytes();
		}
		SetLastWrite(writtenTotal);

		return size == bufferOffset;
	}
}
