#pragma once
#include "Common.h"
#include "IStream.h"
#include "kxf/General/OptionalPtr.h"

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
			
			template<class T>
			DelegateStreamBase(std::unique_ptr<T> stream)
				:m_Stream(std::move(stream))
			{
				static_assert(std::is_base_of_v<TBaseStream, T>, "must inherit from 'TBaseStream' (wx[Input/Output]Stream)");
			}

			DelegateStreamBase(DelegateStreamBase&&) noexcept = default;
			DelegateStreamBase(const DelegateStreamBase&) = delete;

		public:
			bool IsTargetStreamOwned() const
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
			ErrorCode GetLastError() const override
			{
				return m_Stream->GetLastError();
			}
			void Close() override
			{
				m_Stream->Close();
			}

			bool IsSeekable() const override
			{
				return m_Stream->IsSeekable();
			}
			BinarySize GetSize() const override
			{
				return m_Stream->GetSize();
			}
	};
}

namespace kxf
{
	class KX_API InputStreamDelegate final: public Private::DelegateStreamBase<IInputStream>
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
			BinarySize LastRead() const override
			{
				return m_Stream->LastRead();
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

			StreamOffset TellI() const override
			{
				return m_Stream->TellI();
			}
			StreamOffset SeekI(StreamOffset offset, IOStreamSeek seek) override
			{
				return m_Stream->SeekI(offset.GetBytes(), seek);
			}

		public:
			InputStreamDelegate& operator=(InputStreamDelegate&& other) noexcept
			{
				static_cast<DelegateStreamBase&>(*this) = std::move(other);
				return *this;
			}
			InputStreamDelegate& operator=(const InputStreamDelegate&) = delete;
	};

	class KX_API OutputStreamDelegate final: public Private::DelegateStreamBase<IOutputStream>
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
			BinarySize LastWrite() const override
			{
				return m_Stream->LastWrite();
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

			StreamOffset TellO() const override
			{
				return m_Stream->TellO();
			}
			StreamOffset SeekO(StreamOffset offset, IOStreamSeek seek) override
			{
				return m_Stream->SeekO(offset.GetBytes(), seek);
			}

			bool Flush() override
			{
				return m_Stream->Flush();
			}
			bool SetAllocationSize(BinarySize allocationSize) override
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
