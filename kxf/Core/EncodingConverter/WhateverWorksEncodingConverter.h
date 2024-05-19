#pragma once
#include "../Common.h"
#include "../IEncodingConverter.h"

namespace kxf
{
	class WhateverWorksEncodingConverter final: public IEncodingConverter
	{
		protected:
			// IEncodingConverter
			size_t ToWideCharBuffer(std::span<const std::byte> source, std::span<wchar_t> destination) override;
			size_t ToMultiByteBuffer(std::span<const wchar_t> source, std::span<std::byte> destination) override;

		public:
			// IEncodingConverter
			String GetEncodingName() const override;
	};
}
