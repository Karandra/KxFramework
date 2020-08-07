#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include <unordered_map>

namespace kxf
{
	class XMLDocument;
}

namespace kxf
{
	class KX_API AndroidLocalizationPackage: public RTTI::ImplementInterface<AndroidLocalizationPackage, ILocalizationPackage>
	{
		private:
			std::unordered_map<ResourceID, LocalizationItem> m_Items;
			Locale m_Locale;

		private:
			bool DoLoad(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme);

		public:
			AndroidLocalizationPackage() = default;
			AndroidLocalizationPackage(const AndroidLocalizationPackage&) = delete;
			AndroidLocalizationPackage(AndroidLocalizationPackage&&) noexcept = default;

		public:
			// ILocalizationPackage
			Locale GetLocale() const noexcept override
			{
				return m_Locale;
			}
			size_t GetItemCount() const override
			{
				return m_Items.size();
			}

			LocalizationItem GetItem(const ResourceID& id) const override
			{
				auto it = m_Items.find(id);
				if (it != m_Items.end())
				{
					return it->second;
				}
				return {};
			}
			size_t EnumItems(std::function<bool(const ResourceID&, const LocalizationItem&)> func) const override
			{
				size_t count = 0;
				for (const auto& [id, item]: m_Items)
				{
					count++;
					if (!std::invoke(func, id, item))
					{
						break;
					}
				}
				return count;
			}

			bool Load(wxInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override;
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override;

		public:
			AndroidLocalizationPackage& operator=(const AndroidLocalizationPackage&) = delete;
			AndroidLocalizationPackage& operator=(AndroidLocalizationPackage&&) noexcept = default;
	};
}
