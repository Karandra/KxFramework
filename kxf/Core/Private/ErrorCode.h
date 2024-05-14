#pragma once
#include "../Common.h"

namespace kxf
{
	class IErrorCode;
}

namespace kxf::Private
{
	String FormatErrorCode(const IErrorCode& error);
}
