/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStreamWrappers.h"

//////////////////////////////////////////////////////////////////////////
class KxLZ4
{
	public:
		static wxString GetLibraryName();
		static wxString GetLibraryVersion();

	public:
		static constexpr size_t CompressBound(size_t sourceSize)
		{
			// Directly from 'LZ4_COMPRESSBOUND' macro
			constexpr const size_t LZ4_MAX_INPUT_SIZE = 0x7E000000u; // 2 113 929 216 bytes
			return sourceSize > LZ4_MAX_INPUT_SIZE ? 0 : sourceSize + (sourceSize / 255) + 16;
		}
		static size_t Compress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize);
		static KxUInt8Vector Compress(const void* sourceBuffer, size_t sourceSize);

		static size_t Decompress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize);
		static KxUInt8Vector Decompress(const void* sourceBuffer, size_t sourceSize);
};

//////////////////////////////////////////////////////////////////////////
class KxLZ4BaseStream: public KxStreamBase
{
	protected:
		constexpr static const size_t ms_BlockSize = 1024 * 8;

	private:
		uint8_t m_Buffer[2][ms_BlockSize] = {{0}, {0}};
		void* m_LZ4Stream = NULL;

	protected:
		bool HasStream() const
		{
			return m_LZ4Stream;
		}
		void* GetStream() const
		{
			return m_LZ4Stream;
		}
		template<class T> T* GetStream() const
		{
			return reinterpret_cast<T*>(m_LZ4Stream);
		}
		void SetStream(void* stream)
		{
			m_LZ4Stream = stream;
		}

		void* GetBuffer(size_t index)
		{
			return m_Buffer[index];
		}
		const void* GetBuffer(size_t index) const
		{
			return m_Buffer[index];
		}
		size_t GetBufferSize() const
		{
			return ms_BlockSize;
		}
		
		size_t NextBufferIndex(size_t index)
		{
			return (index + 1) % 2;
		}

	public:
		KxLZ4BaseStream();
		virtual ~KxLZ4BaseStream();

	public:
		virtual bool Flush() override
		{
			return false;
		}
		virtual bool SetAllocationSize(Offset offset = InvalidOffset) override
		{
			return false;
		}

		bool SetDictionary(const void* data, size_t size);
		bool SetDictionary(const wxMemoryBuffer& buffer);
};

//////////////////////////////////////////////////////////////////////////
class KxLZ4InputStream:
	public KxLZ4BaseStream,
	public KxInputStreamWrapper<wxFilterInputStream>
{
	public:
		using DictionaryBuffer = std::vector<uint8_t>;

	private:
		uint8_t m_StreamObject[32] = {0};
		DictionaryBuffer m_Dictionary;
		size_t m_RingBufferIndex = 0;

	private:
		void Init();

	protected:
		virtual size_t OnSysRead(void* buffer, size_t size) override;

	public:
		KxLZ4InputStream(wxInputStream& stream, const DictionaryBuffer& dictionary = DictionaryBuffer())
			:KxInputStreamWrapper(stream), m_Dictionary(dictionary)
		{
			Init();
		}
		KxLZ4InputStream(wxInputStream* stream, const DictionaryBuffer& dictionary = DictionaryBuffer())
			:KxInputStreamWrapper(stream), m_Dictionary(dictionary)
		{
			Init();
		}

	public:
		virtual bool IsWriteable() const override
		{
			return false;
		}
		virtual bool IsReadable() const override
		{
			return true;
		}
};

//////////////////////////////////////////////////////////////////////////
class KxLZ4OutputStream:
	public KxLZ4BaseStream,
	public KxOutputStreamWrapper<wxFilterOutputStream>
{
	private:
		int m_Acceleration = 0;

	public:
		KxLZ4OutputStream(wxOutputStream& stream, int acceleration = 0)
			:KxOutputStreamWrapper(stream)
		{
			SetAcceleration(acceleration);
		}
		KxLZ4OutputStream(wxOutputStream* stream, int acceleration = 0)
			:KxOutputStreamWrapper(stream)
		{
			SetAcceleration(acceleration);
		}
	
	public:
		virtual bool IsWriteable() const override
		{
			return true;
		}
		virtual bool IsReadable() const override
		{
			return false;
		}

		int GetAcceleration() const;
		void SetAcceleration(int value);
};
