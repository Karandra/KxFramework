#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include <vector>

namespace KxFramework
{
	class KX_API SecretValue final
	{
		private:
			std::vector<uint8_t> m_Storage;

		public:
			SecretValue() noexcept = default;
			SecretValue(void* data, size_t size)
				:m_Storage(size, 0)
			{
				std::memcpy(m_Storage.data(), data, size);
			}
			SecretValue(const String& string)
			{
				auto utf8 = string.ToUTF8();

				m_Storage.resize(utf8.length(), 0);
				std::memcpy(m_Storage.data(), utf8.data(), utf8.length());
			}
			SecretValue(const SecretValue& other) = delete;
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
			
			String ToString(const wxMBConv& conv = wxConvWhateverWorks) const;
			void Wipe() noexcept;

		public:
			SecretValue& operator=(const SecretValue& other) = delete;
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
