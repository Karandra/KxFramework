#pragma once
#include "Common.h"
#include "IStream.h"
#include "kxf/Core/OptionalPtr.h"

namespace kxf::Private
{
	template<class TBaseStream_>
	class DelegateStreamBase: public TBaseStream_
	{
		public:
			using TBaseStream = TBaseStream_;

		protected:
			optional_ptr<TBaseStream> m_Stream;

		protected:
			DelegateStreamBase() = default;
			DelegateStreamBase(std::nullptr_t)
			{
			}
			DelegateStreamBase(TBaseStream& stream)
				:m_Stream(stream)
			{
			}
			
			template<class T> requires(std::is_base_of_v<TBaseStream, T>)
			DelegateStreamBase(std::unique_ptr<T> stream)
				:m_Stream(std::move(stream))
			{
			}

			DelegateStreamBase(DelegateStreamBase&&) noexcept = default;
			DelegateStreamBase(const DelegateStreamBase&) = delete;

		public:
			bool OwnsTargetStream() const
			{
				return m_Stream.is_owned();
			}
			bool HasTargetStream() const
			{
				return !m_Stream.is_null();
			}
			std::unique_ptr<TBaseStream> TakeTargetStream()
			{
				return m_Stream.get_unique();
			}
			TBaseStream* GetTargetStream() const
			{
				return m_Stream.get();
			}

			const TBaseStream& operator*() const
			{
				return *m_Stream;
			}
			TBaseStream& operator*()
			{
				return *m_Stream;
			}
			TBaseStream* operator->() const
			{
				return m_Stream.get();
			}

			DelegateStreamBase& operator=(DelegateStreamBase&&) noexcept = default;
			DelegateStreamBase& operator=(const DelegateStreamBase&) = delete;

			explicit operator bool() const
			{
				return HasTargetStream();
			}
			bool operator!() const
			{
				return !HasTargetStream();
			}

		public:
			// IStream
			void Close() override
			{
				m_Stream->Close();
			}
			
			StreamError GetLastError() const override
			{
				return m_Stream->GetLastError();
			}
			void SetLastError(StreamError lastError) override
			{
				m_Stream->SetLastError(std::move(lastError));
			}

			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}
			DataSize GetSize() const override
			{
				return m_Stream->GetSize();
			}
	};
}

namespace kxf
{
	class KX_API InputStreamDelegate: public Private::DelegateStreamBase<IInputStream>
	{
		public:
			InputStreamDelegate() = default;
			InputStreamDelegate(const InputStreamDelegate&) = delete;
			InputStreamDelegate(InputStreamDelegate&& other)
				:DelegateStreamBase(std::move(other))
			{
			}

			template<class... Args>
			InputStreamDelegate(Args&&... arg)
				:DelegateStreamBase(std::forward<Args>(arg)...)
			{
			}

		public:
			// IInputStream
			bool CanRead() const override
			{
				return m_Stream->CanRead();
			}
			DataSize LastRead() const override
			{
				return m_Stream->LastRead();
			}
			void SetLastRead(DataSize lastRead) override
			{
				m_Stream->SetLastRead(lastRead);
			}

			std::optional<uint8_t> Peek() override
			{
				return m_Stream->Peek();
			}
			IInputStream& Read(void* buffer, size_t size) override
			{
				m_Stream->Read(buffer, size);
				return *this;
			}
			IInputStream& Read(IOutputStream& other) override
			{
				m_Stream->Read(other);
				return *this;
			}
			bool ReadAll(void* buffer, size_t size) override
			{
				return m_Stream->ReadAll(buffer, size);
			}

			DataSize TellI() const override
			{
				return m_Stream->TellI();
			}
			DataSize SeekI(DataSize offset, IOStreamSeek seek) override
			{
				return m_Stream->SeekI(offset.ToBytes(), seek);
			}

		public:
			InputStreamDelegate& operator=(InputStreamDelegate&& other) noexcept
			{
				static_cast<DelegateStreamBase&>(*this) = std::move(other);
				return *this;
			}
			InputStreamDelegate& operator=(const InputStreamDelegate&) = delete;
	};

	class KX_API OutputStreamDelegate: public Private::DelegateStreamBase<IOutputStream>
	{
		public:
			OutputStreamDelegate() = default;
			OutputStreamDelegate(const OutputStreamDelegate&) = delete;
			OutputStreamDelegate(OutputStreamDelegate&& other)
				:DelegateStreamBase(std::move(other))
			{
			}

			template<class... Args>
			OutputStreamDelegate(Args&&... arg)
				:DelegateStreamBase(std::forward<Args>(arg)...)
			{
			}

		public:
			// IOutputStream
			DataSize LastWrite() const override
			{
				return m_Stream->LastWrite();
			}
			void SetLastWrite(DataSize lastWrite) override
			{
				m_Stream->SetLastWrite(lastWrite);
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				m_Stream->Write(buffer, size);
				return *this;
			}
			IOutputStream& Write(IInputStream& other) override
			{
				m_Stream->Write(other);
				return *this;
			}
			bool WriteAll(const void* buffer, size_t size) override
			{
				return m_Stream->WriteAll(buffer, size);
			}

			DataSize TellO() const override
			{
				return m_Stream->TellO();
			}
			DataSize SeekO(DataSize offset, IOStreamSeek seek) override
			{
				return m_Stream->SeekO(offset.ToBytes(), seek);
			}

			bool Flush() override
			{
				return m_Stream->Flush();
			}
			bool SetAllocationSize(DataSize allocationSize) override
			{
				return m_Stream->SetAllocationSize(allocationSize);
			}

		public:
			OutputStreamDelegate& operator=(OutputStreamDelegate&& other) noexcept
			{
				static_cast<DelegateStreamBase&>(*this) = std::move(other);
				return *this;
			}
			OutputStreamDelegate& operator=(const OutputStreamDelegate&) = delete;
	};
}
