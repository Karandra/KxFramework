#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/IEncodingConverter.h"
#include <vector>

namespace kxf
{
	class KX_API SecretValue final
	{
		public:
			static SecretValue FromString(const String& string)
			{
				auto utf8 = string.ToUTF8();
				return SecretValue(utf8.data(), utf8.length());
			}
			static SecretValue FromString(const char* data, size_t length = String::npos)
			{
				return FromString(String::FromUTF8(data, length));
			}
			static SecretValue FromString(const wchar_t* data, size_t length = String::npos)
			{
				return FromString(String(data, length));
			}

		private:
			std::vector<uint8_t> m_Storage;

		public:
			SecretValue() noexcept = default;
			SecretValue(void* data, size_t size)
			{
				m_Storage.resize(size);
				std::memcpy(m_Storage.data(), data, size);
			}
			SecretValue(const SecretValue&) = delete;
			SecretValue(SecretValue&& other) noexcept
			{
				*this = std::move(other);
			}
			~SecretValue() noexcept
			{
				Wipe();
			}

		public:
			bool IsEmpty() const
			{
				return m_Storage.empty();
			}
			size_t GetSize() const noexcept
			{
				return m_Storage.size();
			}
			const void* GetData() const noexcept
			{
				return m_Storage.data();
			}
			
			String ToString(IEncodingConverter& encodingConverter = EncodingConverter_WhateverWorks) const;
			void Wipe() noexcept;

		public:
			SecretValue& operator=(const SecretValue&) = delete;
			SecretValue& operator=(SecretValue&& other) noexcept
			{
				m_Storage = std::move(other.m_Storage);
				other.Wipe();
				return *this;
			}

			bool operator==(const SecretValue& other) const noexcept
			{
				return m_Storage == other.m_Storage;
			}
			bool operator!=(const SecretValue& other) const noexcept
			{
				return !(*this == other);
			}

			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}
	};
}
