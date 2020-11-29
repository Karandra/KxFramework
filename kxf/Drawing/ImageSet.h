#pragma once
#include "Common.h"
#include "Image.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/General/String.h"
#include "Image.h"
#include <variant>

namespace kxf
{
	class GDIImageList;
}

namespace kxf
{
	class KX_API ImageSet final
	{
		private:
			std::unordered_map<String, std::variant<Image, GDIBitmap, GDIIcon>> m_Items;

		public:
			ImageSet(size_t initialCount = 0)
			{
				m_Items.reserve(initialCount);
			}
			ImageSet(const ImageSet&) = delete;
			ImageSet(ImageSet&& other) noexcept
				:m_Items(std::move(other.m_Items))
			{
			}

		public:
			size_t GetCount() const noexcept
			{
				return m_Items.size();
			}
			bool Remove(const String& id)
			{
				return m_Items.erase(id) != 0;
			}
			void Clear() noexcept
			{
				m_Items.clear();
			}

			void Set(const String& id, const Image& image)
			{
				m_Items.insert_or_assign(id, image);
			}
			void Set(const String& id, const GDIBitmap& bitmap)
			{
				m_Items.insert_or_assign(id, bitmap);
			}
			void Set(const String& id, const GDIIcon& icon)
			{
				m_Items.insert_or_assign(id, icon);
			}

			Image GetImage(const String& id) const;
			GDIBitmap GetBitmap(const String& id) const;
			GDIIcon GetIcon(const String& id) const;

			Size GetItemSize(const String& id) const;
			const IObject* QueryItem(const String& id) const;
			const Image* QueryImage(const String& id) const;
			const GDIBitmap* QueryBitmap(const String& id) const;
			const GDIIcon* QueryIcon(const String& id) const;

			std::unique_ptr<GDIImageList> CreateImageList(const Size& size) const;

		public:
			ImageSet& operator=(const ImageSet&) = delete;
			ImageSet& operator=(ImageSet&& other) noexcept
			{
				m_Items = std::move(other.m_Items);
				return *this;
			}
	};
}
