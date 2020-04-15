#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include "Kx/FileSystem/FSPath.h"
#include "Kx/FileSystem/FileItem.h"
#include "Kx/FileSystem/IFileSystem.h"
#include "Locale.h"
#include <unordered_map>
class KxLibrary;

namespace KxFramework
{
	class KX_API LocalizationPack
	{
		public:
			static const LocalizationPack& GetActive() noexcept;
			static const LocalizationPack& SetActive(const LocalizationPack& localizationPack) noexcept;

		private:
			std::unordered_map<String, String> m_StringTable;
			Locale m_Locale;
			String m_Author;
			String m_Description;

		public:
			LocalizationPack() = default;
			LocalizationPack(const LocalizationPack&) = delete;
			LocalizationPack(LocalizationPack&&) noexcept = default;
			virtual ~LocalizationPack() = default;

		public:
			bool IsEmpty() const noexcept
			{
				return m_StringTable.empty() || m_Locale.IsNull();
			}
			Locale GetLocale() const noexcept
			{
				return m_Locale;
			}
			String GetAuthor() const
			{
				return m_Author;
			}
			String GetDescription() const
			{
				return m_Description;
			}

			std::optional<String> GetString(const String& id) const
			{
				auto it = m_StringTable.find(id);
				if (it != m_StringTable.end())
				{
					return it->second;
				}
				return {};
			}
			std::optional<String> GetString(StandardID id) const
			{
				return Localization::GetStandardLocalizedString(id);
			}
			std::optional<String> GetString(int id) const
			{
				return Localization::GetStandardLocalizedString(id);
			}

			bool Load(const String& xml, Locale locale);
			bool Load(wxInputStream& stream, Locale locale);
			bool Load(const KxLibrary& library, const FSPath& name, Locale locale = {});

		public:
			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}

			LocalizationPack& operator=(const LocalizationPack&) = delete;
			LocalizationPack& operator=(LocalizationPack&&) noexcept = default;
	};
}

namespace KxFramework::Localization
{
	size_t SearchLocalizationPacks(const IFileSystem& fileSystem, const FSPath& directory, std::function<bool(Locale, FileItem)> func);
	size_t SearchLocalizationPacks(const KxLibrary& library, std::function<bool(Locale, FileItem)> func);
}
