#pragma once
#include "Common.h"
#include "Kx/General/String.h"

namespace KxFramework::Localization
{
	struct LangID final
	{
		uint16_t ID = 0;
		uint16_t SortOrder = 0;
	};
}

namespace KxFramework
{
	class KX_API Locale final
	{
		public:
			static Locale GetUserDefault() noexcept;
			static Locale GetSystemDefault() noexcept;
			static Locale GetSystemPreferred() noexcept;
			static Locale GetThreadPreferred() noexcept;

			static Locale FromLCID(uint32_t lcid) noexcept;
			static Locale FromLangID(Localization::LangID langID) noexcept;

		private:
			wchar_t m_LocaleName[128] = {};

		public:
			Locale() noexcept = default;
			Locale(std::string_view localeName) noexcept;
			Locale(std::wstring_view localeName) noexcept;
			Locale(const char* localeName) noexcept
				:Locale(std::string_view(localeName))
			{
			}
			Locale(const wchar_t* localeName) noexcept
				:Locale(std::wstring_view(localeName))
			{
			}
			Locale(const String& localeName) noexcept
				:Locale(StringViewOf(localeName))
			{
			}

		public:
			bool IsNull() const noexcept;
			String GetName() const
			{
				return m_LocaleName;
			}

			std::optional<String> GetOption(LocaleStrOption option) const;
			std::optional<uint32_t> GetOption(LocaleIntOption option) const noexcept;

			bool SetOption(LocaleStrOption option, const String& value);
			bool SetOption(LocaleIntOption option, uint32_t value);

			std::optional<uint32_t> GetLCID() const noexcept;
			std::optional<Localization::LangID> GetLangID() const noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const Locale& other) noexcept
			{
				return std::wstring_view(m_LocaleName) == std::wstring_view(other.m_LocaleName);
			}
			bool operator!=(const Locale& other) noexcept
			{
				return std::wstring_view(m_LocaleName) != std::wstring_view(other.m_LocaleName);
			}
	};
}
