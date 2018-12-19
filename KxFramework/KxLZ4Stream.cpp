/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxLZ4Stream.h"
#include "LZ4/lz4.h"

#if defined _WIN64
#pragma comment(lib, "KxFramework/LZ4/liblz4 x64")
#else
#pragma comment(lib, "KxFramework/LZ4/liblz4 x86")
#endif

namespace LZ4Const
{
	constexpr const int MaxDictionarySize = 64;
	constexpr const size_t BlockSize = 1024 * 64;
}
namespace
{
	LZ4_stream_t* AsCompressorStream(void* stream)
	{
		return reinterpret_cast<LZ4_stream_t*>(stream);
	}
	LZ4_streamDecode_t* AsDecompressorStream(void* stream)
	{
		return reinterpret_cast<LZ4_streamDecode_t*>(stream);
	}
}

//////////////////////////////////////////////////////////////////////////
wxString KxLZ4::GetLibraryName()
{
	return wxS("LZ4");
}
wxString KxLZ4::GetLibraryVersion()
{
	return LZ4_versionString();
}

size_t KxLZ4::Compress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize)
{
	int size = LZ4_compress_default(reinterpret_cast<const char*>(sourceBuffer), reinterpret_cast<char*>(destinationBuffer), sourceSize, destinationSize);
	return size > 0 ? size : 0;
}
KxUInt8Vector KxLZ4::Compress(const void* sourceBuffer, size_t sourceSize)
{
	KxUInt8Vector destinationBuffer;
	destinationBuffer.resize(CompressBound(sourceSize));

	size_t resultSize = Compress(sourceBuffer, sourceSize, destinationBuffer.data(), destinationBuffer.size());
	destinationBuffer.resize(resultSize);

	return destinationBuffer;
}

size_t KxLZ4::Decompress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize)
{
	int size = LZ4_decompress_safe(reinterpret_cast<const char*>(sourceBuffer), reinterpret_cast<char*>(destinationBuffer), sourceSize, destinationSize);
	return size > 0 ? size : 0;
}
KxUInt8Vector KxLZ4::Decompress(const void* sourceBuffer, size_t sourceSize)
{
	KxUInt8Vector destinationBuffer;
	destinationBuffer.resize(sourceSize);

	size_t resultSize = Decompress(sourceBuffer, sourceSize, destinationBuffer.data(), destinationBuffer.size());
	destinationBuffer.resize(resultSize);

	return destinationBuffer;
}

//////////////////////////////////////////////////////////////////////////
KxLZ4BaseStream::KxLZ4BaseStream()
{
}
KxLZ4BaseStream::~KxLZ4BaseStream()
{
}

bool KxLZ4BaseStream::SetDictionary(const void* data, size_t size)
{
	if (m_LZ4Stream)
	{
		return LZ4_loadDict(AsCompressorStream(m_LZ4Stream), reinterpret_cast<const char*>(data), size) <= LZ4Const::MaxDictionarySize;
	}
	return false;
}
bool KxLZ4BaseStream::SetDictionary(const wxMemoryBuffer& buffer)
{
	return SetDictionary(buffer.GetData(), buffer.GetDataLen());
}

//////////////////////////////////////////////////////////////////////////
void KxLZ4InputStream::Init()
{
	// Set stream pointer
	SetStream(m_StreamObject.data());

	// Init stream with or without dictionary
	if (!m_Dictionary.empty())
	{
		LZ4_setStreamDecode(m_StreamObject.GetAs<LZ4_streamDecode_t>(), reinterpret_cast<const char*>(m_Dictionary.data()), m_Dictionary.size());
	}
	else
	{
		LZ4_setStreamDecode(m_StreamObject.GetAs<LZ4_streamDecode_t>(), nullptr, 0);
	}
}

size_t KxLZ4InputStream::OnSysRead(void* buffer, size_t size)
{
	size_t totalProcessedCounter = 0;
	for (size_t compressedCounter = 0; compressedCounter <= size; compressedCounter += ms_BlockSize)
	{
		uint8_t tempData[LZ4_COMPRESSBOUND(ms_BlockSize)] = {0};
		size_t tempDataLength = std::min(size, ms_BlockSize);
		m_parent_i_stream->Read(tempData, tempDataLength);

		void* ringBuffer = GetBuffer(m_RingBufferIndex);
		m_RingBufferIndex = NextBufferIndex(m_RingBufferIndex);

		int processedSize = LZ4_decompress_safe_continue(GetStream<LZ4_streamDecode_t>(),
														 reinterpret_cast<const char*>(tempData),
														 reinterpret_cast<char*>(ringBuffer),
														 tempDataLength,
														 ms_BlockSize);

		if (processedSize > 0)
		{
			std::memcpy(reinterpret_cast<uint8_t*>(buffer) + totalProcessedCounter, tempData, processedSize);
			totalProcessedCounter += (size_t)processedSize;
			
			continue;
		}

		m_lasterror = wxSTREAM_EOF;
		break;
	}
	return size;
}

KxLZ4InputStream::KxLZ4InputStream(wxInputStream& stream, const DictionaryBuffer& dictionary)
	:KxInputStreamWrapper(stream), m_Dictionary(dictionary)
{
	Init();
}
KxLZ4InputStream::KxLZ4InputStream(wxInputStream* stream, const DictionaryBuffer& dictionary)
	:KxInputStreamWrapper(stream), m_Dictionary(dictionary)
{
	Init();
}
KxLZ4InputStream::~KxLZ4InputStream()
{
}

//////////////////////////////////////////////////////////////////////////
KxLZ4OutputStream::KxLZ4OutputStream(wxOutputStream& stream, int acceleration)
	:KxOutputStreamWrapper(stream)
{
	SetAcceleration(acceleration);
}
KxLZ4OutputStream::KxLZ4OutputStream(wxOutputStream* stream, int acceleration)
	: KxOutputStreamWrapper(stream)
{
	SetAcceleration(acceleration);
}
KxLZ4OutputStream::~KxLZ4OutputStream()
{

}

int KxLZ4OutputStream::GetAcceleration() const
{
	return m_Acceleration;
}
void KxLZ4OutputStream::SetAcceleration(int value)
{
	m_Acceleration = std::clamp(value, 0, 9);
}
