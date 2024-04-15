#pragma once
#include "../Common.h"
#include "../IEncodingConverter.h"

namespace kxf
{
	class NativeEncodingConverter final: public IEncodingConverter
	{
		private:
			int m_CodePage = -1;
			FlagSet<uint32_t> m_Flags;

		protected:
			size_t ToMultiByteBuffer(std::span<const wchar_t> source, std::span<std::byte> destination) override;
			size_t ToWideCharBuffer(std::span<const std::byte> source, std::span<wchar_t> destination) override;

		public:
			NativeEncodingConverter(int codePage, FlagSet<uint32_t> flags = {}) noexcept
				:m_CodePage(codePage), m_Flags(flags)
			{
			}
			~NativeEncodingConverter() = default;

		public:
			String GetEncodingName() const override;
	};
}
