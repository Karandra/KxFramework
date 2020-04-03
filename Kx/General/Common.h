#pragma once
#include <Kx/Common.hpp>
#include "Kx/General/EnumClass.h"

namespace KxFramework
{
	enum class VersionType
	{
		None = -1,

		Default,
		DateTime,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowCast(VersionType);
	}
}
