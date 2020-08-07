#pragma once
#include "../Common.h"
#include <unordered_map>

namespace kxf::Localization::Private
{
	class KX_API PackageHelper final
	{
		public:
			using TItems = std::unordered_map<ResourceID, LocalizationItem>;

		private:
			TItems* m_Items = nullptr;

		public:
			PackageHelper(TItems& items)
				:m_Items(&items)
			{
			}
			PackageHelper(const PackageHelper&) = delete;
			PackageHelper(PackageHelper&&) noexcept = default;

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

			template<class TFunc>
			size_t EnumItems(TFunc&& func) const
			{
				size_t count = 0;
				for (const auto& [id, item]: *m_Items)
				{
					count++;
					if (!std::invoke(func, id, item))
					{
						break;
					}
				}
				return count;
			}

		public:
			PackageHelper& operator=(const PackageHelper&) = delete;
			PackageHelper& operator=(PackageHelper&& other) noexcept = default;
	};
}
