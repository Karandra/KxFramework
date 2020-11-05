#include "stdafx.h"
#include "ImageSet.h"
#include "ImageList.h"
#include "kxf/Utility/Common.h"

namespace
{
	template<class T>
	const T* GetPtr(const kxf::ImageSet& imageSet, const kxf::String& id, kxf::ImageSetItemType desiredType) noexcept
	{
		using namespace kxf;

		ImageSetItemType type = ImageSetItemType::None;
		if (const kxf::IObject* object = imageSet.QueryItem(id, &type); object && type == desiredType)
		{
			return object->QueryInterface<T>().get();
		}
		return nullptr;
	}
}

namespace kxf
{
	wxIMPLEMENT_DYNAMIC_CLASS(ImageSet, wxObject);

	Image ImageSet::GetImage(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const IObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Image:
				{
					return *object->QueryInterface<Image>();
				}
				case ItemType::Bitmap:
				{
					return object->QueryInterface<Bitmap>()->ToImage();
				}
				case ItemType::Icon:
				{
					return object->QueryInterface<Icon>()->ToImage();
				}
			};
		}
		return {};
	}
	Bitmap ImageSet::GetBitmap(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const IObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Image:
				{
					return object->QueryInterface<Image>()->ToBitmap();
				}
				case ItemType::Bitmap:
				{
					return *object->QueryInterface<Bitmap>();
				}
				case ItemType::Icon:
				{
					return object->QueryInterface<Icon>()->ToBitmap();
				}
			};
		}
		return {};
	}
	Icon ImageSet::GetIcon(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const IObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Image:
				{
					return object->QueryInterface<Image>()->ToIcon();
				}
				case ItemType::Bitmap:
				{
					return object->QueryInterface<Bitmap>()->ToIcon();
				}
				case ItemType::Icon:
				{
					return *object->QueryInterface<Icon>();
				}
			};
		}
		return {};
	}

	Size ImageSet::GetItemSize(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const IObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Bitmap:
				{
					return object->QueryInterface<Bitmap>()->GetSize();
				}
				case ItemType::Image:
				{
					return object->QueryInterface<Image>()->GetSize();
				}
				case ItemType::Icon:
				{
					return object->QueryInterface<Icon>()->GetSize();
				}
			};
		};
		return Size::UnspecifiedSize();
	}
	const IObject* ImageSet::QueryItem(const String& id, ItemType* type) const
	{
		if (auto it = m_Items.find(id); it != m_Items.end())
		{
			const auto& item = it->second;
			switch (static_cast<ItemType>(item.index()))
			{
				case ItemType::Bitmap:
				{
					Utility::SetIfNotNull(type, ItemType::Bitmap);
					return &std::get<Bitmap>(item);
				}
				case ItemType::Image:
				{
					Utility::SetIfNotNull(type, ItemType::Image);
					return &std::get<Image>(item);
				}
				case ItemType::Icon:
				{
					Utility::SetIfNotNull(type, ItemType::Icon);
					return &std::get<Icon>(item);
				}
			};
		}

		Utility::SetIfNotNull(type, ItemType::None);
		return nullptr;
	}
	const Image* ImageSet::QueryImage(const String& id) const
	{
		return GetPtr<Image>(*this, id, ItemType::Image);
	}
	const Bitmap* ImageSet::QueryBitmap(const String& id) const
	{
		return GetPtr<Bitmap>(*this, id, ItemType::Bitmap);
	}
	const Icon* ImageSet::QueryIcon(const String& id) const
	{
		return GetPtr<Icon>(*this, id, ItemType::Icon);
	}

	std::unique_ptr<ImageList> ImageSet::CreateImageList(const Size& size) const
	{
		std::unique_ptr<ImageList> imageList = std::make_unique<ImageList>(size, GetCount());

		for (const auto& item: m_Items)
		{
			ItemType type = ItemType::None;
			if (const IObject* object = QueryItem(item.first, &type))
			{
				switch (type)
				{
					case ItemType::Image:
					{
						imageList->Add(*object->QueryInterface<Image>());
						break;
					}
					case ItemType::Bitmap:
					{
						imageList->Add(*object->QueryInterface<Bitmap>());
						break;
					}
					case ItemType::Icon:
					{
						imageList->Add(*object->QueryInterface<Icon>());
						break;
					}
				};
			}
		}

		return imageList;
	}
}
