#pragma once
#include "Common.h"
#include "ILocalizationPackage.h"
#include "kxf/Utility/Container.h"
#include "kxf/Utility/WithOptionalOwnership.h"

namespace kxf
{
	class KX_API LocalizationPackageStack: public RTTI::ImplementInterface<LocalizationPackageStack, ILocalizationPackage>
	{
		private:
			template<class TItems, class TFunc>
			static size_t DoEnumLocalizationPackages(TItems&& items, TFunc&& func)
			{
				size_t count = 0;
				for (auto it = std::rbegin(items); it != std::rend(items); ++it)
				{
					auto&& item = *it;
					if (item)
					{
						count++;
						if (!std::invoke(*item))
						{
							break;
						}
					}
				}
				return count;
			}

		private:
			std::vector<Utility::WithOptionalOwnership<ILocalizationPackage>> m_Packages;

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

		public:
			// ILocalizationPackage
			size_t GetItemCount() const override
			{
				size_t count = 0;
				EnumLocalizationPackages([&](const ILocalizationPackage& localizationPackage)
				{
					count += localizationPackage.GetItemCount();
					return true;
				});
				return count;
			}
			LocalizationItem GetItem(const ResourceID& id) const override
			{
				LocalizationItem item;
				EnumLocalizationPackages([&](const ILocalizationPackage& localizationPackage)
				{
					item = localizationPackage.GetItem(id);
					return item.IsNull();
				});
				return item;
			}
			size_t EnumItems(std::function<bool(ResourceID, LocalizationItem)> func) const override
			{
				size_t count = 0;
				EnumLocalizationPackages([&](const ILocalizationPackage& localizationPackage)
				{
					bool canContinue = false;
					count += localizationPackage.EnumItems([&](ResourceID id, LocalizationItem item)
					{
						canContinue = std::invoke(func, std::move(id), std::move(item));
						return canContinue;
					});
					return canContinue;
				});
				return count;
			}

			bool Load(wxInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
			{
				return false;
			}
			bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale = {}, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) override
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
					return item.Get() == &localizationPackage;
				}) == m_Packages.end();
			}
			Utility::WithOptionalOwnership<ILocalizationPackage> Detach(const ILocalizationPackage& localizationPackage)
			{
				auto it = Utility::Container::FindIf(m_Packages, [&](const auto& item)
				{
					return item.Get() == &localizationPackage;
				});
				if (it != m_Packages.end())
				{
					auto item = std::move(*it);
					m_Packages.erase(it);
					return item;
				}
				return {};
			}

			template<class TFunc>
			size_t EnumLocalizationPackages(TFunc&& func) const noexcept
			{
				return DoEnumLocalizationPackages(m_Packages, std::forward<TFunc>(func));
			}

			template<class TFunc>
			size_t EnumLocalizationPackages(TFunc&& func) noexcept
			{
				return DoEnumLocalizationPackages(m_Packages, std::forward<TFunc>(func));
			}

		public:
			explicit operator bool() const
			{
				return !m_Packages.empty() && !IsEmpty();
			}
			bool operator!() const
			{
				return m_Packages.empty() || IsEmpty();
			}
	};
}
