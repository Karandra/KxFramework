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
	class KX_API WindowsLocalizationPackage:
		public RTTI::ImplementInterface<WindowsLocalizationPackage, ILocalizationPackage>,
		private Localization::Private::XMLPackageHelper
	{
		private:
			Localization::Private::ItemsPackageHelper m_PackageHelper;
			std::unordered_map<ResourceID, LocalizationItem> m_Items;
			Locale m_Locale;

		private:
			bool DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme);
			void DoSetLocale(const Locale& locale) override
			{
				m_Locale = locale;
			}

		public:
			WindowsLocalizationPackage()
				:m_PackageHelper(m_Items)
			{
			}
			WindowsLocalizationPackage(const WindowsLocalizationPackage&) = delete;
			WindowsLocalizationPackage(WindowsLocalizationPackage&&) noexcept = default;

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

			bool Load(wxInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(stream, locale, loadingScheme);
			}
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return XMLPackageHelper::Load(library, name, locale, loadingScheme);
			}

		public:
			WindowsLocalizationPackage& operator=(const WindowsLocalizationPackage&) = delete;
			WindowsLocalizationPackage& operator=(WindowsLocalizationPackage&&) noexcept = default;
	};
}
