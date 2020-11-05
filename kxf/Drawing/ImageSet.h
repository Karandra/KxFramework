#pragma once
#include "Common.h"
#include "Bitmap.h"
#include "Image.h"
#include "Icon.h"
#include "kxf/General/String.h"
#include <variant>

namespace kxf
{
	class ImageList;

	enum class ImageSetItemType
	{
		None = -1,

		Bitmap,
		Image,
		Icon,
	};
}

namespace kxf
{
	class KX_API ImageSet: public wxObject
	{
		public:
			using ItemType = ImageSetItemType;

		private:
			std::unordered_map<String, std::variant<Bitmap, Image, Icon>> m_Items;
		
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
			void Set(const String& id, const Bitmap& bitmap)
			{
				m_Items.insert_or_assign(id, bitmap);
			}
			void Set(const String& id, const Icon& icon)
			{
				m_Items.insert_or_assign(id, icon);
			}

			Image GetImage(const String& id) const;
			Bitmap GetBitmap(const String& id) const;
			Icon GetIcon(const String& id) const;

			Size GetItemSize(const String& id) const;
			const IObject* QueryItem(const String& id, ItemType* type = nullptr) const;
			const Image* QueryImage(const String& id) const;
			const Bitmap* QueryBitmap(const String& id) const;
			const Icon* QueryIcon(const String& id) const;

			std::unique_ptr<ImageList> CreateImageList(const Size& size) const;

		public:
			ImageSet& operator=(const ImageSet&) = delete;
			ImageSet& operator=(ImageSet&& other) noexcept
			{
				m_Items = std::move(other.m_Items);
				return *this;
			}
		
		public:
			wxDECLARE_DYNAMIC_CLASS(ImageSet);
	};
}
