#pragma once
#include "../Common.h"
#include "../HTTPStatus.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/IO/IStream.h"

namespace kxf
{
	class CURLSession;
}

namespace kxf
{
	class KX_API ICURLReply: public RTTI::Interface<ICURLReply>
	{
		KxRTTI_DeclareIID(ICURLReply, {0x29650f53, 0xbc2b, 0x48d1, {0x91, 0xe2, 0xe1, 0xe, 0x71, 0xf6, 0x70, 0x8e}});

		protected:
			virtual void AddProcessedData(const void* data, size_t size) = 0;
			virtual size_t GetProcessed() const = 0;

		public:
			virtual ~ICURLReply() = default;

		public:
			virtual bool IsSuccess() const = 0;

			virtual int GetErrorCode() const = 0;
			virtual String GetErrorMessage() const = 0;
			virtual HTTPStatus GetResponseCode() const = 0;

		public:
			explicit operator bool() const noexcept
			{
				return IsSuccess();
			}
			bool operator!() const noexcept
			{
				return !IsSuccess();
			}
	};
}

namespace kxf
{
	class KX_API CURLReplyBase: public ICURLReply
	{
		friend class CURLSession;

		private:
			enum class ErrorCode
			{
				Success = 0,
				Invalid = -2
			};

		private:
			String m_ErrorMessage;
			HTTPStatus m_ResponseCode;
			int m_ErrorCode = static_cast<int>(ErrorCode::Invalid);

		protected:
			void SetErrorCode(int code)
			{
				m_ErrorCode = code;
			}
			void SetResponseCode(int code)
			{
				m_ResponseCode = code;
			}
			void SetErrorMessage(const String& value)
			{
				m_ErrorMessage = value;
			}

		public:
			bool IsSuccess() const override
			{
				return m_ErrorCode == static_cast<int>(ErrorCode::Success) && m_ResponseCode.IsSuccess();
			}

			int GetErrorCode() const override
			{
				return m_ErrorCode;
			}
			String GetErrorMessage() const override;
			HTTPStatus GetResponseCode() const override
			{
				return m_ResponseCode;
			}
	};
}

namespace kxf
{
	class KX_API CURLStringReply: public CURLReplyBase
	{
		friend class CURLSession;

		private:
			String m_Buffer;
			size_t m_Downloaded = 0;

		private:
			void AddProcessedData(StringView data)
			{
				m_Buffer += data;
				m_Downloaded += data.length() * sizeof(StringView::value_type);
			}
			void AddProcessedData(const void* data, size_t size) override
			{
				m_Buffer += std::string_view(static_cast<const char*>(data), size);
				m_Downloaded += size;
			}
			size_t GetProcessed() const override
			{
				return m_Downloaded;
			}

		public:
			bool IsSuccess() const override
			{
				return !m_Buffer.IsEmpty() && CURLReplyBase::IsSuccess();
			}

			const String& GetString() const& noexcept
			{
				return m_Buffer;
			}
			String& GetString()& noexcept
			{
				return m_Buffer;
			}

			operator const String&() const&
			{
				return m_Buffer;
			}
			operator String&()&
			{
				return m_Buffer;
			}
			operator String()&&
			{
				return std::move(m_Buffer);
			}
	};
}

namespace kxf
{
	class KX_API CURLBinaryReply: public CURLReplyBase
	{
		friend class CURLSession;

		private:
			wxMemoryBuffer m_Buffer;

		private:
			void AddProcessedData(const void* data, size_t size) override
			{
				m_Buffer.AppendData(data, size);
			}
			size_t GetProcessed() const override
			{
				return m_Buffer.GetDataLen();
			}

		public:
			CURLBinaryReply()
				:m_Buffer(0)
			{
			}

		public:
			bool IsSuccess() const override
			{
				return !m_Buffer.IsEmpty() && CURLReplyBase::IsSuccess();
			}

			const void* GetData() const
			{
				return m_Buffer.GetData();
			}
			size_t GetSize() const
			{
				return m_Buffer.GetDataLen();
			}

			String ToString() const
			{
				return String::FromUTF8(static_cast<const char*>(m_Buffer.GetData()), m_Buffer.GetDataLen());
			}
	};
}

namespace kxf
{
	class KX_API CURLStreamReply: public CURLReplyBase
	{
		friend class CURLSession;

		private:
			IOutputStream& m_Stream;
			size_t m_Downloaded = 0;
			StreamOffset m_InitialOffset;

		private:
			void AddProcessedData(const void* data, size_t size) override
			{
				m_Stream.WriteAll(data, size);
				m_Downloaded += size;
			}
			size_t GetProcessed() const override
			{
				return m_InitialOffset.ToBytes() + m_Downloaded;
			}

		public:
			CURLStreamReply(IOutputStream& stream, StreamOffset initialPos = {})
				:m_Stream(stream), m_InitialOffset(initialPos)
			{
				if (m_InitialOffset)
				{
					m_Stream.SeekO(m_InitialOffset, IOStreamSeek::FromStart);
				}
			}

		public:
			bool IsSuccess() const override
			{
				return m_Stream && CURLReplyBase::IsSuccess();
			}

			StreamOffset GetResumeOffset() const
			{
				return m_InitialOffset;
			}
			bool ShouldResumeFromOffset(StreamOffset& pos) const
			{
				pos = m_InitialOffset;
				return m_InitialOffset.IsValid();
			}
			bool ShouldResumeFromOffset() const
			{
				StreamOffset offset;
				return ShouldResumeFromOffset(offset);
			}

			const IOutputStream& GetStream() const
			{
				return m_Stream;
			}
			IOutputStream& GetStream()
			{
				return m_Stream;
			}
	};
}
