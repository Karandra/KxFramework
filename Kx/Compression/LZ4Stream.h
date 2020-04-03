#pragma once
#include "Common.h"
#include "Kx/General/StreamWrappers.h"
#include "Kx/General/Version.h"

namespace KxFramework::Compression::LZ4
{
	wxString GetLibraryName();
	Version GetLibraryVersion();

	inline constexpr size_t CompressBound(size_t sourceSize)
	{
		// Copied directly from 'LZ4_COMPRESSBOUND' macro
		constexpr const size_t LZ4_MAX_INPUT_SIZE = 0x7E000000u; // 2 113 929 216 bytes
		return sourceSize > LZ4_MAX_INPUT_SIZE ? 0 : sourceSize + (sourceSize / 255) + 16;
	}

	size_t Compress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize);
	std::vector<uint8_t> Compress(const void* sourceBuffer, size_t sourceSize);

	size_t Decompress(const void* sourceBuffer, size_t sourceSize, void* destinationBuffer, size_t destinationSize);
	std::vector<uint8_t> Decompress(const void* sourceBuffer, size_t sourceSize);
}

namespace KxFramework
{
	class KX_API LZ4BaseStream: public IStreamWrapper
	{
		protected:
			constexpr static const size_t ms_BlockSize = 1024 * 8;

		private:
			uint8_t m_Buffer[2][ms_BlockSize] = {{0}, {0}};
			void* m_LZ4Stream = nullptr;

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
			LZ4BaseStream() = default;
			~LZ4BaseStream() = default;

		public:
			bool Flush() override
			{
				return false;
			}
			bool SetAllocationSize(BinarySize offset = {}) override
			{
				return false;
			}

			bool SetDictionary(const void* data, size_t size);
			bool SetDictionary(const wxMemoryBuffer& buffer)
			{
				return SetDictionary(buffer.GetData(), buffer.GetDataLen());
			}
	};
}

namespace KxFramework
{
	class KX_API LZ4InputStream: public LZ4BaseStream, public InputStreamWrapper<wxFilterInputStream>
	{
		public:
			using DictionaryBuffer = std::vector<uint8_t>;

		private:
			uint8_t m_StreamObject[64] = {};
			DictionaryBuffer m_Dictionary;
			size_t m_RingBufferIndex = 0;

		private:
			void Init();

		protected:
			size_t OnSysRead(void* buffer, size_t size) override;

		public:
			LZ4InputStream(wxInputStream& stream, const DictionaryBuffer& dictionary = {})
				:InputStreamWrapper(stream), m_Dictionary(dictionary)
			{
				Init();
			}
			LZ4InputStream(wxInputStream* stream, const DictionaryBuffer& dictionary = {})
				:InputStreamWrapper(stream), m_Dictionary(dictionary)
			{
				Init();
			}

		public:
			bool IsWriteable() const override
			{
				return false;
			}
			bool IsReadable() const override
			{
				return true;
			}
	};
}

namespace KxFramework
{
	class KX_API LZ4OutputStream: public LZ4BaseStream, public OutputStreamWrapper<wxFilterOutputStream>
	{
		private:
			int m_Acceleration = 0;

		public:
			LZ4OutputStream(wxOutputStream& stream, int acceleration = 0)
				:OutputStreamWrapper(stream)
			{
				SetAcceleration(acceleration);
			}
			LZ4OutputStream(wxOutputStream* stream, int acceleration = 0)
				:OutputStreamWrapper(stream)
			{
				SetAcceleration(acceleration);
			}

		public:
			bool IsWriteable() const override
			{
				return true;
			}
			bool IsReadable() const override
			{
				return false;
			}

			int GetAcceleration() const
			{
				return m_Acceleration;
			}
			void SetAcceleration(int value)
			{
				m_Acceleration = std::clamp(value, 0, 9);
			}
	};
}
