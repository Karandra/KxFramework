#include "KxfPCH.h"
#include "LocalizationPackageStack.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/Utility/Enumerator.h"

namespace kxf
{
	size_t LocalizationPackageStack::GetItemCount() const
	{
		size_t count = 0;
		for (const ILocalizationPackage& package: EnumLocalizationPackages())
		{
			count += package.GetItemCount();
		}
		return count;
	}
	const LocalizationItem& LocalizationPackageStack::GetItem(const ResourceID& id) const
	{
		for (const ILocalizationPackage& package: EnumLocalizationPackages())
		{
			if (const LocalizationItem& item = package.GetItem(id))
			{
				return item;
			}
		}
		return NullLocalizationItem;
	}
	Enumerator<ILocalizationPackage::ItemRef> LocalizationPackageStack::EnumItems() const
	{
		using TEnum = Enumerator<ILocalizationPackage::ItemRef>;

		return [this, packageIt = m_Packages.rbegin(), packageEnumerator = TEnum(), enumeratorIt = TEnum::iterator()]() mutable -> TEnum::TValueContainer
		{
			if (packageIt != m_Packages.rend())
			{
				// Init items for the current package
				if (!packageEnumerator)
				{
					packageEnumerator = (*packageIt)->EnumItems();
				}

				if (packageEnumerator)
				{
					// Init iterator for current items set
					if (enumeratorIt == packageEnumerator.end())
					{
						enumeratorIt = packageEnumerator.begin();
					}

					// Get the item
					decltype(auto) item = *enumeratorIt;

					// If we have reached the end, reset the enumerator and advance to the next package
					if (++enumeratorIt == packageEnumerator.end())
					{
						packageEnumerator = {};
						++packageIt;
					}
					return item;
				}
			}
			return {};
		};
	}

	Enumerator<const ILocalizationPackage&> LocalizationPackageStack::EnumLocalizationPackages() const noexcept
	{
		return Utility::EnumerateIterableContainerReverse<const ILocalizationPackage&, Utility::ReferenceOf>(m_Packages);
	}
	Enumerator<ILocalizationPackage&> LocalizationPackageStack::EnumLocalizationPackages() noexcept
	{
		return Utility::EnumerateIterableContainerReverse<ILocalizationPackage&, Utility::ReferenceOf>(m_Packages);
	}
	Enumerator<String> LocalizationPackageStack::EnumFileExtensions() const
	{
		return {};
	}
}
