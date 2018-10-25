/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxMemoryStream.h"

namespace
{
	bool FlushBuffer(wxStreamBuffer* streamBuffer)
	{
		return streamBuffer->FlushBuffer();
	}
	bool SetBufferAllocationSize(wxStreamBuffer* streamBuffer, KxStreamBase::Offset offset)
	{
		size_t currentPos = streamBuffer->GetIntPosition();
		if ((KxStreamBase::Offset)currentPos > offset)
		{
			if (offset != KxStreamBase::InvalidOffset)
			{
				streamBuffer->SetIntPosition(offset);
			}
			streamBuffer->Truncate();

			return true;
		}
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
bool KxMemoryInputStream::Flush()
{
	return FlushBuffer(GetInputStreamBuffer());
}
bool KxMemoryInputStream::SetAllocationSize(Offset offset)
{
	return SetBufferAllocationSize(GetInputStreamBuffer(), offset);
}

//////////////////////////////////////////////////////////////////////////
bool KxMemoryOutputStream::Flush()
{
	return FlushBuffer(GetOutputStreamBuffer());
}
bool KxMemoryOutputStream::SetAllocationSize(Offset offset)
{
	return SetBufferAllocationSize(GetOutputStreamBuffer(), offset);
}
