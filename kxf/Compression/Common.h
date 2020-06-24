#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/LocallyUniqueID.h"
#include "kxf/General/UniversallyUniqueID.h"

namespace kxf::Compression
{
	using FileIndex = size_t;
	using FileIndexVector = std::vector<FileIndex>;

	constexpr FileIndex InvalidFileIndex = std::numeric_limits<FileIndex>::max();
}
