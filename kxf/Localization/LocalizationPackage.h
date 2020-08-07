#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/ResourceID.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/FileSystem/IFileSystem.h"
#include "Locale.h"
#include <unordered_map>
#include <wx/stream.h>

namespace kxf
{
	class DynamicLibrary;
}

namespace kxf
{
	class KX_API LocalizationPackage
	{
		public:
			static const LocalizationPackage& GetActive() noexcept;
			static const LocalizationPackage& SetActive(const LocalizationPackage& package) noexcept;

		private:
			std::unordered_map<String, String> m_StringTable;
			Locale m_Locale;
			String m_Author;
			String m_Description;

		public:
			LocalizationPackage() = default;
			LocalizationPackage(const LocalizationPackage&) = delete;
			LocalizationPackage(LocalizationPackage&&) noexcept = default;
			virtual ~LocalizationPackage() = default;

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
			std::optional<String> GetString(StdID id) const
			{
				return Localization::GetStandardString(id);
			}
			std::optional<String> GetString(int id) const
			{
				return Localization::GetStandardString(id);
			}

			bool Load(const String& xml, const Locale& locale);
			bool Load(wxInputStream& stream, const Locale& locale);
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale = {});

		public:
			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}

			LocalizationPackage& operator=(const LocalizationPackage&) = delete;
			LocalizationPackage& operator=(LocalizationPackage&&) noexcept = default;
	};
}
