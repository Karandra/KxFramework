#include "stdafx.h"
#include "IStream.h"

namespace
{
	constexpr size_t g_BufferSize = kxf::BinarySize::FromKB(64).GetBytes();
}

namespace kxf::IO
{
	BinarySize ReadCopy(IInputStream& inStream, IOutputStream& outStream)
	{
		BinarySize totalBytes = 0;
		uint8_t buffer[g_BufferSize];

		while (true)
		{
			BinarySize read = inStream.Read(buffer, std::size(buffer)).LastRead();
			if (!read)
			{
				break;
			}
			else if (outStream.Write(buffer, read.GetBytes()).LastWrite() != read)
			{
				break;
			}

			totalBytes += read;
		}
		return totalBytes;
	}
	bool ReadAll(IInputStream& stream, void* buffer, size_t size, BinarySize& readTotal)
	{
		size_t bufferOffset = 0;
		while (true)
		{
			const BinarySize lastRead = stream.Read(static_cast<uint8_t*>(buffer) + bufferOffset, size - bufferOffset).LastRead();

			// There is no point in continuing looping if we can't read anything at all
			if (!lastRead)
			{
				break;
			}
			readTotal += lastRead;

			// Or if an error occurred on the stream
			if (stream.GetLastError().IsFail())
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
			bufferOffset += lastRead.GetBytes();
		}

		return size == bufferOffset;
	}
	bool WriteAll(IOutputStream& stream, const void* buffer, size_t size, BinarySize& writtenTotal)
	{
		// This function mirrors implementation of 'ReadAll', look there for more comments.

		size_t bufferOffset = 0;
		while (true)
		{
			const BinarySize lastWrite = stream.Write(buffer, size).LastWrite();
			if (!lastWrite)
			{
				break;
			}
			writtenTotal += lastWrite;

			if (stream.GetLastError().IsFail())
			{
				break;
			}
			if (lastWrite >= size)
			{
				size = 0;
				break;
			}

			bufferOffset += lastWrite.GetBytes();
		}

		return size == bufferOffset;
	}
}
