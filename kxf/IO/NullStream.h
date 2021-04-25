#pragma once
#include "Common.h"
#include "IStream.h"

namespace kxf
{
	class NullStream final: public IStream
	{
		public:
			static IStream& Get();

		public:
			// IStream
			void Close() override
			{
			}

			StreamError GetLastError() const override
			{
				return StreamError::Fail();
			}
			void SetLastError(StreamError lastError) override
			{
			}

			bool IsSeekable() const override
			{
				return false;
			}
			BinarySize GetSize() const override
			{
				return {};
			}
	};
}

namespace kxf
{
	class NullInputStream final: public IInputStream
	{
		public:
			static IInputStream& Get();

		public:
			// IStream
			void Close() override
			{
			}

			StreamError GetLastError() const override
			{
				return StreamError::Fail();
			}
			void SetLastError(StreamError lastError) override
			{
			}

			bool IsSeekable() const override
			{
				return false;
			}
			BinarySize GetSize() const override
			{
				return {};
			}

		public:
			// IInputStream
			bool CanRead() const override
			{
				return false;
			}

			BinarySize LastRead() const override
			{
				return {};
			}
			void SetLastRead(BinarySize lastRead) override
			{
			}

			std::optional<uint8_t> Peek() override
			{
				return {};
			}
			IInputStream& Read(void* buffer, size_t size) override
			{
				return *this;
			}
			IInputStream& Read(IOutputStream& other) override
			{
				return *this;
			}
			bool ReadAll(void* buffer, size_t size) override
			{
				return false;
			}

			StreamOffset TellI() const override
			{
				return {};
			}
			StreamOffset SeekI(StreamOffset offset, IOStreamSeek seek) override
			{
				return {};
			}
	};
}

namespace kxf
{
	class NullOutputStream final: public IOutputStream
	{
		public:
			static IOutputStream& Get();

		public:
			// IStream
			void Close() override
			{
			}

			StreamError GetLastError() const override
			{
				return StreamError::Fail();
			}
			void SetLastError(StreamError lastError) override
			{
			}

			bool IsSeekable() const override
			{
				return false;
			}
			BinarySize GetSize() const override
			{
				return {};
			}

		public:
			BinarySize LastWrite() const override
			{
				return {};
			}
			void SetLastWrite(BinarySize lastWrite) override
			{
			}

			IOutputStream& Write(const void* buffer, size_t size) override
			{
				return *this;
			}
			IOutputStream& Write(IInputStream& other) override
			{
				return *this;
			}
			bool WriteAll(const void* buffer, size_t size) override
			{
				return false;
			}

			StreamOffset TellO() const override
			{
				return {};
			}
			StreamOffset SeekO(StreamOffset offset, IOStreamSeek seek) override
			{
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
	};
}
