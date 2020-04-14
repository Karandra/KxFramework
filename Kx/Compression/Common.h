#pragma once
#include "Kx/Common.hpp"

namespace KxFramework::Compression
{
	using FileIndex = uint32_t;
	using FileIndexVector = std::vector<FileIndex>;

	constexpr FileIndex InvalidFileIndex = std::numeric_limits<FileIndex>::max();
}
