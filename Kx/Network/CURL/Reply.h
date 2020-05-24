#pragma once
#include "../Common.h"
#include "../HTTPStatusCode.h"
#include "Kx/RTTI/QueryInterface.h"

namespace kxf
{
	class CURLSession;
}

namespace kxf
{
	class KX_API ICURLReply: public RTTI::Interface<ICURLReply>
	{
		KxDecalreIID(ICURLReply, {0x29650f53, 0xbc2b, 0x48d1, {0x91, 0xe2, 0xe1, 0xe, 0x71, 0xf6, 0x70, 0x8e}});

		protected:
			virtual void AddProcessedData(const void* data, size_t size) = 0;
			virtual size_t GetProcessed() const = 0;

		public:
			virtual ~ICURLReply() = default;

		public:
			virtual bool IsSuccess() const = 0;

			virtual int GetErrorCode() const = 0;
			virtual String GetErrorMessage() const = 0;
			virtual HTTPStatusCode GetResponseCode() const = 0;

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
			HTTPStatusCode m_ResponseCode;
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
			HTTPStatusCode GetResponseCode() const override
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
			wxOutputStream& m_Stream;
			size_t m_Downloaded = 0;
			int64_t m_InitialPosition = 0;

		private:
			void AddProcessedData(const void* data, size_t size) override
			{
				m_Stream.WriteAll(data, size);
				m_Downloaded += size;
			}
			size_t GetProcessed() const override
			{
				return m_InitialPosition + m_Downloaded;
			}

		public:
			CURLStreamReply(wxOutputStream& stream, int64_t initialPos = 0)
				:m_Stream(stream), m_InitialPosition(initialPos)
			{
				if (m_InitialPosition > 0)
				{
					m_Stream.SeekO(m_InitialPosition);
				}
			}

		public:
			bool IsSuccess() const override
			{
				return m_Stream.IsOk() && CURLReplyBase::IsSuccess();
			}

			int64_t GetResumeFromPosition() const
			{
				return m_InitialPosition;
			}
			bool ShouldResumeFromPosition(int64_t& pos) const
			{
				pos = m_InitialPosition;
				return m_InitialPosition > 0;
			}
			bool ShouldResumeFromPosition() const
			{
				int64_t pos;
				return ShouldResumeFromPosition(pos);
			}

			const wxOutputStream& GetStream() const
			{
				return m_Stream;
			}
			wxOutputStream& GetStream()
			{
				return m_Stream;
			}
	};
}
