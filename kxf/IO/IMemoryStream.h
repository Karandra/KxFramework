#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"
class wxStreamBuffer;

namespace kxf
{
	class KX_API IMemoryStream: public RTTI::Interface<IMemoryStream>
	{
		KxRTTI_DeclareIID(IMemoryStream, {0x3e20ad70, 0x8357, 0x4470, {0xac, 0x30, 0xe5, 0xe9, 0xd4, 0x7d, 0xe8, 0x14}});

		public:
			virtual wxStreamBuffer& GetStreamBuffer() const = 0;
	};
}
