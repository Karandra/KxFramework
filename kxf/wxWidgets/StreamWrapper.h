#pragma once
#include "Common.h"
#include "kxf/General/OptionalPtr.h"
#include "kxf/IO/IStream.h"
#include <wx/stream.h>

namespace kxf::wxWidgets
{
	class InputStreamWrapper: public RTTI::ImplementInterface<InputStreamWrapper, IInputStream>
	{
		protected:
			optional_ptr<wxInputStream> m_Stream;
			BinarySize m_LastRead;
			ErrorCode m_LastError;

		protected:
			virtual void InvalidateInputCache()
			{
				m_LastRead = {};
				m_LastError = {};
			}

		public:
			InputStreamWrapper(wxInputStream& stream)
				:m_Stream(stream)
			{
			}
			InputStreamWrapper(std::unique_ptr<wxInputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// IStream
			void Close() override
			{
				// Can't close input stream
			}

			ErrorCode GetLastError() const override;
			void SetLastError(ErrorCode lastError) override
			{
				m_LastError = lastError;
			}

			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}
			BinarySize GetSize() const override
			{
				BinarySize size = m_Stream->GetLength();
				if (!size)
				{
					size = m_Stream->GetSize();
				}

				return size;
			}

			// IInputStream
			bool CanRead() const override
			{
				return m_Stream->CanRead();
			}
			BinarySize LastRead() const override
			{
				return m_LastRead ? m_LastRead : m_Stream->LastRead();
			}
			void SetLastRead(BinarySize lastRead) override
			{
				m_LastRead = lastRead;
			}

			std::optional<uint8_t> Peek() override
			{
				InvalidateInputCache();
				return m_Stream->Peek();
			}
			IInputStream& Read(void* buffer, size_t size) override
			{
				InvalidateInputCache();
				m_Stream->Read(buffer, size);
				return *this;
			}

			StreamOffset TellI() const override
			{
				return m_Stream->TellI();
			}
			StreamOffset SeekI(StreamOffset offset, IOStreamSeek seek) override
			{
				InvalidateInputCache();

				if (auto seekWx = IO::ToWxSeekMode(seek))
				{
					return m_Stream->SeekI(offset.ToBytes(), *seekWx);
				}
				return {};
			}

			// InputStreamWrapper
			wxInputStream& AsWxStream() noexcept
			{
				return *m_Stream;
			}
			const wxInputStream& AsWxStream() const noexcept
			{
				return *m_Stream;
			}
	};

	class KX_API OutputStreamWrapper: public RTTI::ImplementInterface<OutputStreamWrapper, IOutputStream>
	{
		private:
			optional_ptr<wxOutputStream> m_Stream;
			BinarySize m_LastWrite;
			ErrorCode m_LastError;

		protected:
			virtual void InvalidateOutputCache()
			{
				m_LastWrite = {};
				m_LastError = {};
			}

		public:
			OutputStreamWrapper(wxOutputStream& stream)
				:m_Stream(stream)
			{
			}
			OutputStreamWrapper(std::unique_ptr<wxOutputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// IStream
			void Close() override
			{
				static_cast<void>(m_Stream->Close());
			}

			ErrorCode GetLastError() const override;
			void SetLastError(ErrorCode lastError) override
			{
				m_LastError = lastError;
			}

			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}
			BinarySize GetSize() const override
			{
				BinarySize size = m_Stream->GetLength();
				if (!size)
				{
					size = m_Stream->GetSize();
				}

				return size;
			}

			// IOutputStream
			BinarySize LastWrite() const override
			{
				return m_LastWrite ? m_LastWrite : m_Stream->LastWrite();
			}
			void SetLastWrite(BinarySize lastWrite) override
			{
				m_LastWrite = lastWrite;
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				InvalidateOutputCache();
				m_Stream->Write(buffer, size);
				return *this;
			}

			StreamOffset TellO() const override
			{
				return m_Stream->TellO();
			}
			StreamOffset SeekO(StreamOffset offset, IOStreamSeek seek) override
			{
				InvalidateOutputCache();

				if (auto seekWx = IO::ToWxSeekMode(seek))
				{
					return m_Stream->SeekO(offset.ToBytes(), *seekWx);
				}
				return {};
			}

			bool Flush() override
			{
				return false;
			}
			bool SetAllocationSize(BinarySize allocationSize) override
			{
				return false;
			}

			// OutputStream
			wxOutputStream& AsWxStream() noexcept
			{
				return *m_Stream;
			}
			const wxOutputStream& AsWxStream() const noexcept
			{
				return *m_Stream;
			}
	};
}

namespace kxf::wxWidgets
{
	class InputStreamWrapperWx: public wxInputStream
	{
		protected:
			optional_ptr<IInputStream> m_Stream;

		protected:
			// wxStreamBase
			wxFileOffset OnSysTell() const override
			{
				return m_Stream->TellI().ToBytes<wxFileOffset>();
			}
			wxFileOffset OnSysSeek(wxFileOffset offset, wxSeekMode mode) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekI(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// wxInputStream
			size_t OnSysRead(void* buffer, size_t size) override
			{
				return m_Stream->Read(buffer, size).LastRead().ToBytes<size_t>();
			}

		public:
			InputStreamWrapperWx(IInputStream& stream)
				:m_Stream(stream)
			{
			}
			InputStreamWrapperWx(std::unique_ptr<IInputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// wxStreamBase
			bool IsOk() const override
			{
				return static_cast<bool>(*m_Stream);
			}
			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}

			wxFileOffset GetLength() const override
			{
				return m_Stream->GetSize().ToBytes<wxFileOffset>();
			}
			size_t GetSize() const override
			{
				return m_Stream->GetSize().ToBytes<size_t>();
			}

			// wxInputStream
			bool Eof() const override
			{
				return !m_Stream->CanRead();
			}
			bool CanRead() const override
			{
				return m_Stream->CanRead();
			}

			size_t LastRead() const override
			{
				return m_Stream->LastRead().ToBytes<size_t>();
			}
			wxInputStream& Read(void* buffer, size_t size) override
			{
				m_Stream->Read(buffer, size);
				return *this;
			}
			char Peek() override
			{
				return m_Stream->Peek().value_or(0xFFu);
			}

			wxFileOffset TellI() const override
			{
				return m_Stream->TellI().ToBytes<wxFileOffset>();
			}
			wxFileOffset SeekI(wxFileOffset offset, wxSeekMode mode = wxSeekMode::wxFromStart) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekI(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// InputStreamWrapperWx
			IInputStream& AsKxfStream() noexcept
			{
				return *m_Stream;
			}
			const IInputStream& AsKxfStream() const noexcept
			{
				return *m_Stream;
			}
	};

	class OutputStreamWrapperWx: public wxOutputStream
	{
		protected:
			optional_ptr<IOutputStream> m_Stream;

		protected:
			// wxStreamBase
			wxFileOffset OnSysTell() const override
			{
				return m_Stream->TellO().ToBytes<wxFileOffset>();
			}
			wxFileOffset OnSysSeek(wxFileOffset offset, wxSeekMode mode) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekO(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// wxOutputStream
			size_t OnSysWrite(const void* buffer, size_t size) override
			{
				return m_Stream->Write(buffer, size).LastWrite().ToBytes<size_t>();
			}

		public:
			OutputStreamWrapperWx(IOutputStream& stream)
				:m_Stream(stream)
			{
			}
			OutputStreamWrapperWx(std::unique_ptr<IOutputStream> stream)
				:m_Stream(std::move(stream))
			{
			}

		public:
			// wxStreamBase
			bool IsOk() const override
			{
				return static_cast<bool>(*m_Stream);
			}
			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}

			wxFileOffset GetLength() const override
			{
				return m_Stream->GetSize().ToBytes<wxFileOffset>();
			}
			size_t GetSize() const override
			{
				return m_Stream->GetSize().ToBytes<size_t>();
			}

			// wxOutputStream
			bool Close() override
			{
				const bool wasOk = IsOk();
				m_Stream->Close();
				return wasOk;
			}

			size_t LastWrite() const override
			{
				return m_Stream->LastWrite().ToBytes<size_t>();
			}
			wxOutputStream& Write(const void* buffer, size_t size) override
			{
				m_Stream->Write(buffer, size);
				return *this;
			}

			wxFileOffset TellO() const override
			{
				return m_Stream->TellO().ToBytes<wxFileOffset>();
			}
			wxFileOffset SeekO(wxFileOffset offset, wxSeekMode mode = wxSeekMode::wxFromStart) override
			{
				if (auto seek = IO::FromWxSeekMode(mode))
				{
					return m_Stream->SeekO(offset, *seek).ToBytes<wxFileOffset>();
				}
				return wxInvalidOffset;
			}

			// OutputStreamWrapperWx
			IOutputStream& AsKxfStream() noexcept
			{
				return *m_Stream;
			}
			const IOutputStream& AsKxfStream() const noexcept
			{
				return *m_Stream;
			}
	};
}
