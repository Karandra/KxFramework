#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include <variant>

namespace KxFramework
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

namespace KxFramework
{
	class KX_API ImageSet: public wxObject
	{
		public:
			using ItemType = ImageSetItemType;

		private:
			std::unordered_map<String, std::variant<wxBitmap, wxImage, wxIcon>> m_Items;
		
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

			void Set(const String& id, const wxImage& image)
			{
				m_Items.insert_or_assign(id, image);
			}
			void Set(const String& id, const wxBitmap& bitmap)
			{
				m_Items.insert_or_assign(id, bitmap);
			}
			void Set(const String& id, const wxIcon& icon)
			{
				m_Items.insert_or_assign(id, icon);
			}

			wxImage GetImage(const String& id) const;
			wxBitmap GetBitmap(const String& id) const;
			wxIcon GetIcon(const String& id) const;

			wxSize GetItemSize(const String& id) const;
			const wxObject* QueryItem(const String& id, ItemType* type = nullptr) const;
			const wxImage* QueryImage(const String& id) const;
			const wxBitmap* QueryBitmap(const String& id) const;
			const wxIcon* QueryIcon(const String& id) const;

			std::unique_ptr<ImageList> CreateImageList(const wxSize& size) const;

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
