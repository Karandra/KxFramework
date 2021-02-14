#pragma once
#include "Common.h"
#include "HashValue.h"
#include "kxf/General/String.h"
#include "kxf/IO/IStream.h"

namespace kxf::Crypto
{
	KX_API size_t Rot13(String& source) noexcept;
	KX_API size_t UwUize(String& source) noexcept;

	KX_API std::optional<HashValue<32>> CRC32(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<128>> MD5(IInputStream& stream) noexcept;

	KX_API std::optional<HashValue<160>> SHA1(IInputStream& stream) noexcept;

	KX_API std::optional<HashValue<224>> SHA2_224(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<256>> SHA2_256(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<384>> SHA2_384(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<512>> SHA2_512(IInputStream& stream) noexcept;

	KX_API std::optional<HashValue<224>> SHA3_224(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<256>> SHA3_256(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<384>> SHA3_384(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<512>> SHA3_512(IInputStream& stream) noexcept;

	KX_API std::optional<HashValue<32>> xxHash_32(const void* data, size_t size) noexcept;
	KX_API std::optional<HashValue<64>> xxHash_64(const void* data, size_t size) noexcept;
	KX_API std::optional<HashValue<128>> xxHash_128(const void* data, size_t size) noexcept;
	KX_API std::optional<HashValue<32>> xxHash_32(IInputStream& stream) noexcept;
	KX_API std::optional<HashValue<64>> xxHash_64(IInputStream& stream) noexcept;

	KX_API bool Base64Encode(IInputStream& inputStream, IOutputStream& outputStream);
	KX_API bool Base64Decode(IInputStream& inputStream, IOutputStream& outputStream);
};
