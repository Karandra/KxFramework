#pragma once
#include "Common.h"
#include "HashValue.h"
#include "kxf/General/String.h"
#include <wx/stream.h>
#include <array>

namespace kxf::Crypto
{
	size_t Rot13(String& source) noexcept;

	std::optional<HashValue<32>> CRC32(wxInputStream& stream) noexcept;
	std::optional<HashValue<128>> MD5(wxInputStream& stream) noexcept;

	std::optional<HashValue<160>> SHA1(wxInputStream& stream) noexcept;

	std::optional<HashValue<224>> SHA2_224(wxInputStream& stream) noexcept;
	std::optional<HashValue<256>> SHA2_256(wxInputStream& stream) noexcept;
	std::optional<HashValue<384>> SHA2_384(wxInputStream& stream) noexcept;
	std::optional<HashValue<512>> SHA2_512(wxInputStream& stream) noexcept;

	std::optional<HashValue<224>> SHA3_224(wxInputStream& stream) noexcept;
	std::optional<HashValue<256>> SHA3_256(wxInputStream& stream) noexcept;
	std::optional<HashValue<384>> SHA3_384(wxInputStream& stream) noexcept;
	std::optional<HashValue<512>> SHA3_512(wxInputStream& stream) noexcept;

	std::optional<HashValue<32>> xxHash_32(const void* data, size_t size) noexcept;
	std::optional<HashValue<64>> xxHash_64(const void* data, size_t size) noexcept;
	std::optional<HashValue<128>> xxHash_128(const void* data, size_t size) noexcept;
	std::optional<HashValue<32>> xxHash_32(wxInputStream& stream) noexcept;
	std::optional<HashValue<64>> xxHash_64(wxInputStream& stream) noexcept;

	bool Base64Encode(wxInputStream& inputStream, wxOutputStream& outputStream);
	bool Base64Decode(wxInputStream& inputStream, wxOutputStream& outputStream);
};
