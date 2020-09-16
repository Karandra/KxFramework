#pragma once
#include "Common.h"
#include "IStream.h"
#include <wx/mstream.h>

namespace kxf
{
	class KX_API MemoryInputStream: public RTTI::ImplementInterface<MemoryInputStream, IInputStream>
	{
		private:
			wxMemoryInputStream m_Stream;

		public:
			MemoryInputStream(const void* data, size_t size)
				:m_Stream(data, size)
			{
			}
			MemoryInputStream(const wxMemoryOutputStream& stream)
				:m_Stream(stream)
			{
			}
			MemoryInputStream(wxMemoryInputStream& stream)
				:m_Stream(stream)
			{
			}
			MemoryInputStream(wxInputStream& stream, BinarySize size = {})
				:m_Stream(stream, size.GetBytes())
			{
			}

		public:
			// IStream
			ErrorCode GetLastError() const override;
			void Close() override
			{
				// Can't close 'wxInputStream'
			}

			bool IsSeekable() const override
			{
				return m_Stream.IsSeekable();
			}
			BinarySize GetSize() const override
			{
				return m_Stream.GetLength();
			}

			// IInputStream
			bool CanRead() const override
			{
				return m_Stream.CanRead();
			}
			BinarySize LastRead() const override
			{
				return m_Stream.LastRead();
			}
			std::optional<uint8_t> Peek() override
			{
				return m_Stream.Peek();
			}

			IInputStream& Read(void* buffer, size_t size) override
			{
				m_Stream.Read(buffer, size);
				return *this;
			}
			IInputStream& Read(IOutputStream& other) override;
			bool ReadAll(void* buffer, size_t size) override
			{
				return m_Stream.ReadAll(buffer, size);
			}

			StreamOffset TellI() const override
			{
				return m_Stream.TellI();
			}
			StreamOffset SeekI(StreamOffset offset, IOStreamSeek seek) override
			{
				if (auto seekWx = IO::ToWxSeekMode(seek))
				{
					return m_Stream.SeekI(offset.GetBytes(), *seekWx);
				}
				return {};
			}

			// MemoryInputStream
			wxMemoryInputStream& AsWxStream() noexcept
			{
				return m_Stream;
			}
			const wxMemoryInputStream& AsWxStream() const noexcept
			{
				return m_Stream;
			}

			bool Flush();
			bool SetAllocationSize(BinarySize offset);
	};
}

namespace kxf
{
	class KX_API MemoryOutputStream: public RTTI::ImplementInterface<MemoryOutputStream, IOutputStream>
	{
		private:
			wxMemoryOutputStream m_Stream;

		public:
			MemoryOutputStream(void* data = nullptr, size_t size = 0)
				:m_Stream(data, size)
			{
			}

		public:
			// IStream
			ErrorCode GetLastError() const override;
			void Close() override
			{
				static_cast<void>(m_Stream.Close());
			}

			bool IsSeekable() const override
			{
				return m_Stream.IsSeekable();
			}
			BinarySize GetSize() const override
			{
				return m_Stream.GetLength();
			}

			// IOutputStream
			BinarySize LastWrite() const override
			{
				return m_Stream.LastWrite();
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				m_Stream.Write(buffer, size);
				return *this;
			}
			IOutputStream& Write(IInputStream& other) override;
			bool WriteAll(const void* buffer, size_t size) override
			{
				return m_Stream.WriteAll(buffer, size);
			}

			StreamOffset TellO() const override
			{
				return m_Stream.TellO();
			}
			StreamOffset SeekO(StreamOffset offset, IOStreamSeek seek) override
			{
				if (auto seekWx = IO::ToWxSeekMode(seek))
				{
					return m_Stream.SeekO(offset.GetBytes(), *seekWx);
				}
				return {};
			}

			bool Flush() override;
			bool SetAllocationSize(BinarySize allocationSize) override;

			// MemoryOutputStream
			wxMemoryOutputStream& AsWxStream() noexcept
			{
				return m_Stream;
			}
			const wxMemoryOutputStream& AsWxStream() const noexcept
			{
				return m_Stream;
			}
	};
}
