#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include "Private/LocalizationPackageHelper.h"

namespace kxf
{
	class XMLDocument;
}

namespace kxf
{
	class KX_API AndroidLocalizationPackage: public RTTI::ImplementInterface<AndroidLocalizationPackage, ILocalizationPackage>
	{
		private:
			Localization::Private::PackageHelper m_PackageHelper;
			std::unordered_map<ResourceID, LocalizationItem> m_Items;
			Locale m_Locale;

		private:
			bool DoLoad(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme);

		public:
			AndroidLocalizationPackage()
				:m_PackageHelper(m_Items)
			{
			}
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
				if (const LocalizationItem* item = m_PackageHelper.GetItem(id))
				{
					return *item;
				}
				return {};
			}
			size_t EnumItems(std::function<bool(const ResourceID&, const LocalizationItem&)> func) const override
			{
				return m_PackageHelper.EnumItems(func);
			}

			bool Load(wxInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override;
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override;

		public:
			AndroidLocalizationPackage& operator=(const AndroidLocalizationPackage&) = delete;
			AndroidLocalizationPackage& operator=(AndroidLocalizationPackage&&) noexcept = default;
	};
}
