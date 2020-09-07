#pragma once
#include "../Common.h"
#include "kxf/General/String.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/RTTI/QueryInterface.h"
#include <wx/stream.h>

namespace kxf
{
	class KX_API ISystemProcessStdIO: public RTTI::Interface<ISystemProcessStdIO>
	{
		KxRTTI_DeclareIID(ISystemProcessStdIO, {0xbeb81b73, 0x3c92, 0x4e7b, {0x8a, 0x85, 0x7, 0x69, 0x4, 0x24, 0xe3, 0xa8}});

		public:
			virtual ~ISystemProcessStdIO() = default;

		public:
			virtual wxOutputStream& GetStdIn() = 0;
			virtual wxInputStream& GetStdOut() = 0;
			virtual wxInputStream& GetStdErr() = 0;

			virtual String ReadStdOut() const = 0;
			virtual String ReadStdErr() const = 0;
	};
}
