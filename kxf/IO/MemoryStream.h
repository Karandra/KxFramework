#pragma once
#include "Common.h"
#include "IStream.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include <wx/mstream.h>

namespace kxf
{
	class MemoryInputStream;
	class MemoryOutputStream;
}

namespace kxf
{
	class KX_API MemoryInputStream: public wxWidgets::InputStreamWrapper
	{
		private:
			wxMemoryInputStream m_Stream;

		public:
			MemoryInputStream(const void* buffer, size_t size)
				:InputStreamWrapper(m_Stream), m_Stream(buffer, size)
			{
			}
			
			MemoryInputStream(MemoryInputStream& stream)
				:InputStreamWrapper(m_Stream), m_Stream(stream.AsWxStream())
			{
			}
			MemoryInputStream(const MemoryOutputStream& stream);
			MemoryInputStream(IInputStream& stream, BinarySize size = {});

			MemoryInputStream(wxMemoryInputStream& stream)
				:InputStreamWrapper(m_Stream), m_Stream(stream.GetInputStreamBuffer()->GetBufferStart(), stream.GetInputStreamBuffer()->GetBufferSize())
			{
			}
			MemoryInputStream(const wxMemoryOutputStream& stream)
				:InputStreamWrapper(m_Stream), m_Stream(stream.GetOutputStreamBuffer()->GetBufferStart(), stream.GetOutputStreamBuffer()->GetBufferSize())
			{
			}
			MemoryInputStream(wxInputStream& stream, BinarySize size = {})
				:InputStreamWrapper(m_Stream), m_Stream(stream, size.ToBytes())
			{
			}

		public:
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
	class KX_API MemoryOutputStream: public wxWidgets::OutputStreamWrapper
	{
		private:
			wxMemoryOutputStream m_Stream;

		public:
			MemoryOutputStream(void* buffer = nullptr, size_t size = 0)
				:OutputStreamWrapper(m_Stream), m_Stream(buffer, size)
			{
			}

		public:
			// IOutputStream
			bool Flush() override;
			bool SetAllocationSize(BinarySize allocationSize) override;

			// MemoryOutputStream
			size_t CopyTo(void* buffer, size_t size) const;

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
