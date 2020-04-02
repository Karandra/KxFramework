#pragma once
#include "Common.h"
#include "StreamWrappers.h"
#include <wx/mstream.h>

namespace KxFramework
{
	class KX_API MemoryInputStream: public IBaseStreamWrapper, public InputStreamWrapper<wxMemoryInputStream>
	{
		public:
			MemoryInputStream(const void* data, size_t size)
				:InputStreamWrapper(data, size)
			{
			}
			MemoryInputStream(const wxMemoryOutputStream& stream)
				:InputStreamWrapper(stream)
			{
			}
			MemoryInputStream(wxMemoryInputStream& stream)
				:InputStreamWrapper(stream)
			{
			}
			MemoryInputStream(wxInputStream& stream, BinarySize size = {})
				:InputStreamWrapper(stream, size.GetBytes())
			{
			}

		public:
			bool IsReadable() const override
			{
				return true;
			}
			bool IsWriteable() const override
			{
				return false;
			}

			bool Flush() override;
			bool SetAllocationSize(BinarySize offset = {}) override;
	};
}

namespace KxFramework
{
	class KX_API MemoryOutputStream: public IBaseStreamWrapper, public OutputStreamWrapper<wxMemoryOutputStream>
	{
		public:
			MemoryOutputStream(void* data = nullptr, size_t size = 0)
				:OutputStreamWrapper(data, size)
			{
			}

		public:
			bool IsReadable() const override
			{
				return false;
			}
			bool IsWriteable() const override
			{
				return true;
			}

			bool Flush() override;
			bool SetAllocationSize(BinarySize offset = {}) override;
	};
}
