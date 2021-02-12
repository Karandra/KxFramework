#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include "kxf/Utility/Container.h"
#include "kxf/General/OptionalPtr.h"

namespace kxf
{
	class KX_API LocalizationPackageStack: public RTTI::Implementation<LocalizationPackageStack, ILocalizationPackage>
	{
		private:
			std::vector<optional_ptr<ILocalizationPackage>> m_Packages;

		private:
			Locale DoGetLocale() const
			{
				if (!m_Packages.empty())
				{
					return m_Packages.front()->GetLocale();
				}
				return {};
			}

		public:
			LocalizationPackageStack() = default;
			LocalizationPackageStack(ILocalizationPackage& localizationPackage)
			{
				Add(localizationPackage);
			}
			LocalizationPackageStack(std::unique_ptr<ILocalizationPackage> localizationPackage)
			{
				Add(std::move(localizationPackage));
			}
			LocalizationPackageStack(const LocalizationPackageStack&) = delete;
			LocalizationPackageStack(LocalizationPackageStack&&) noexcept = default;

		public:
			// ILocalizationPackage
			Locale GetLocale() const override
			{
				return DoGetLocale();
			}
			size_t GetItemCount() const override;
			const LocalizationItem& GetItem(const ResourceID& id) const override;
			Enumerator<ItemRef> EnumItems() const override;

			bool Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return false;
			}
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return false;
			}

			// LocalizationPackageStack
			ILocalizationPackage& Add(ILocalizationPackage& localizationPackage)
			{
				return *m_Packages.emplace_back(localizationPackage);
			}
			ILocalizationPackage& Add(std::unique_ptr<ILocalizationPackage> localizationPackage)
			{
				return *m_Packages.emplace_back(std::move(localizationPackage));
			}
			bool Remove(const ILocalizationPackage& localizationPackage)
			{
				return Utility::Container::RemoveSingleIf(m_Packages, [&](const auto& item)
				{
					return item.get() == &localizationPackage;
				}) == m_Packages.end();
			}
			optional_ptr<ILocalizationPackage> Detach(const ILocalizationPackage& localizationPackage)
			{
				auto it = Utility::Container::FindIf(m_Packages, [&](const auto& item)
				{
					return item.get() == &localizationPackage;
				});
				if (it != m_Packages.end())
				{
					auto item = std::move(*it);
					m_Packages.erase(it);
					return item;
				}
				return {};
			}

			Enumerator<const ILocalizationPackage&> EnumLocalizationPackages() const noexcept;
			Enumerator<ILocalizationPackage&> EnumLocalizationPackages() noexcept;

		public:
			explicit operator bool() const
			{
				return !m_Packages.empty() && !IsEmpty();
			}
			bool operator!() const
			{
				return m_Packages.empty() || IsEmpty();
			}

			LocalizationPackageStack& operator=(const LocalizationPackageStack&) = delete;
			LocalizationPackageStack& operator=(LocalizationPackageStack&&) noexcept = default;
	};
}
