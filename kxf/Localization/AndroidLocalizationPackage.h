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
	class KX_API AndroidLocalizationPackage:
		public RTTI::ImplementInterface<AndroidLocalizationPackage, ILocalizationPackage>,
		private Localization::Private::XMLPackageHelper
	{
		private:
			Localization::Private::ItemsPackageHelper m_ItemsHelper;
			std::unordered_map<ResourceID, LocalizationItem> m_Items;
			Locale m_Locale;

		private:
			bool DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme) override;
			void DoSetLocale(const Locale& locale) override
			{
				m_Locale = locale;
			}

		public:
			AndroidLocalizationPackage()
				:m_ItemsHelper(m_Items)
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

			const LocalizationItem& GetItem(const ResourceID& id) const override
			{
				if (const LocalizationItem* item = m_ItemsHelper.GetItem(id))
				{
					return *item;
				}
				return NullLocalizationItem;
			}
			size_t EnumItems(std::function<bool(const ResourceID&, const LocalizationItem&)> func) const override
			{
				return m_ItemsHelper.EnumItems(func);
			}

			bool Load(wxInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(stream, locale, loadingScheme);
			}
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(library, name, locale, loadingScheme);
			}

		public:
			AndroidLocalizationPackage& operator=(const AndroidLocalizationPackage&) = delete;
			AndroidLocalizationPackage& operator=(AndroidLocalizationPackage&&) noexcept = default;
	};
}
