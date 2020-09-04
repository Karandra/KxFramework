#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Utility/Container.h"

namespace kxf::Sciter
{
	class Host;
}

namespace kxf::Sciter
{
	class KX_API StylesheetStorage
	{
		private:
			std::vector<String> m_Items;

		public:
			StylesheetStorage() noexcept = default;
			virtual ~StylesheetStorage() = default;

		public:
			bool AddItem(String stylesheet);
			void ClearItems() noexcept;
			size_t GetItemCount() const noexcept
			{
				return m_Items.size();
			}
			size_t CopyItems(const StylesheetStorage& other);

			template<class TFunc>
			size_t EnumItems(TFunc&& func) const&
			{
				size_t count = 0;
				for (const String& stylesheet: m_Items)
				{
					count++;
					if (!std::invoke(func, stylesheet))
					{
						break;
					}
				}
				return count;
			}

			template<class TFunc>
			size_t EnumItems(TFunc&& func) &&
			{
				size_t count = 0;
				for (String&& stylesheet: m_Items)
				{
					count++;
					if (!std::invoke(func, std::move(stylesheet)))
					{
						break;
					}
				}

				Utility::Container::RemoveEachIf(m_Items, [](const String& item)
				{
					return item.IsEmptyOrWhitespace();
				});
				return count;
			}

			bool Apply(Host& host, const FSPath& basePath = {}) const;
			bool Apply(Host& host, const URI& baseURI) const;
	};
}
