#pragma once
#include "Common.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	class KX_API IStreamOnFileSystem: public RTTI::Interface<IStreamOnFileSystem>
	{
		KxRTTI_DeclareIID(IStreamOnFileSystem, {0xf5a807ab, 0xe82a, 0x4215, {0x90, 0xb3, 0xc2, 0x7f, 0xd7, 0x44, 0xfc, 0x2f}});

		public:
			IStreamOnFileSystem() noexcept = default;
			virtual ~IStreamOnFileSystem() = default;

		public:
			virtual FSPath GetPath() const = 0;
			virtual UniversallyUniqueID GetUniqueID() const = 0;
	};
}
