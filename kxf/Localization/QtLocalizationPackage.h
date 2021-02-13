#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include "Private/LocalizationPackageHelper.h"
#include "kxf/General/Version.h"

namespace kxf
{
	class XMLDocument;
}

namespace kxf
{
	class KX_API QtLocalizationPackage:
		public RTTI::DynamicImplementation<QtLocalizationPackage, ILocalizationPackage>,
		private Localization::Private::XMLPackageHelper
	{
		private:
			Localization::Private::ItemsPackageHelper m_ItemsHelper;
			std::unordered_map<ResourceID, LocalizationItem> m_Items;
			Locale m_Locale;
			Version m_Version;

		private:
			bool DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme);
			void DoSetLocale(const Locale& locale) override
			{
				if (locale)
				{
					m_Locale = locale;
				}
			}

		public:
			QtLocalizationPackage()
				:m_ItemsHelper(m_Items)
			{
			}
			QtLocalizationPackage(const QtLocalizationPackage&) = delete;
			QtLocalizationPackage(QtLocalizationPackage&&) noexcept = default;

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
			Enumerator<ItemRef> EnumItems() const override
			{
				return m_ItemsHelper.EnumItems();
			}

			bool Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(stream, locale, loadingScheme);
			}
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(library, name, locale, loadingScheme);
			}
			Enumerator<String> EnumFileExtensions() const override;

			// QtLocalizationPackage
			Version GetVersion() const
			{
				return m_Version;
			}

		public:
			QtLocalizationPackage& operator=(const QtLocalizationPackage&) = delete;
			QtLocalizationPackage& operator=(QtLocalizationPackage&&) noexcept = default;
	};
}
