#pragma once
#include "../Common.h"
#include "../ILocalizationPackage.h"
#include "kxf/Core/Enumerator.h"

namespace kxf
{
	class XMLDocument;
}

namespace kxf::Localization::Private
{
	class KX_API ItemsPackageHelper final
	{
		public:
			using TItems = std::unordered_map<ResourceID, LocalizationItem>;

		private:
			TItems* m_Items = nullptr;

		public:
			ItemsPackageHelper(TItems& items)
				:m_Items(&items)
			{
			}
			ItemsPackageHelper(const ItemsPackageHelper&) = delete;
			ItemsPackageHelper(ItemsPackageHelper&&) noexcept = default;

		public:
			const LocalizationItem* GetItem(const ResourceID& id) const
			{
				auto it = m_Items->find(id);
				if (it != m_Items->end())
				{
					return &it->second;
				}
				return nullptr;
			}
			Enumerator<ILocalizationPackage::ItemRef> EnumItems() const;

		public:
			ItemsPackageHelper& operator=(const ItemsPackageHelper&) = delete;
			ItemsPackageHelper& operator=(ItemsPackageHelper&& other) noexcept = default;
	};

	class KX_API XMLPackageHelper
	{
		protected:
			virtual bool DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme) = 0;
			virtual void DoSetLocale(const Locale& locale) = 0;

		public:
			bool Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace);
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace);
	};
}
