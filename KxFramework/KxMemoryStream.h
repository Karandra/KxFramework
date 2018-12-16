/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStreamWrappers.h"

//////////////////////////////////////////////////////////////////////////
class KX_API KxMemoryInputStream:
	public KxStreamBase,
	public KxInputStreamWrapper<wxMemoryInputStream>
{
	public:
		KxMemoryInputStream(const void* data, size_t size)
			:KxInputStreamWrapper(data, size)
		{
		}
		KxMemoryInputStream(const wxMemoryOutputStream& stream)
			:KxInputStreamWrapper(stream)
		{
		}
		KxMemoryInputStream(wxMemoryInputStream& stream)
			:KxInputStreamWrapper(stream)
		{
		}
		KxMemoryInputStream(wxInputStream& stream, Offset size = InvalidOffset)
			:KxInputStreamWrapper(stream, size)
		{
		}

	public:
		virtual bool IsReadable() const override
		{
			return true;
		}
		virtual bool IsWriteable() const override
		{
			return false;
		}

		virtual bool Flush() override;
		virtual bool SetAllocationSize(Offset offset = InvalidOffset) override;
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxMemoryOutputStream:
	public KxStreamBase,
	public KxOutputStreamWrapper<wxMemoryOutputStream>
{
	public:
		KxMemoryOutputStream(void* data = nullptr, size_t size = 0)
			:KxOutputStreamWrapper(data, size)
		{
		}

	public:
		virtual bool IsReadable() const override
		{
			return false;
		}
		virtual bool IsWriteable() const override
		{
			return true;
		}

		virtual bool Flush() override;
		virtual bool SetAllocationSize(Offset offset = InvalidOffset) override;
};
