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
		if (const wxObject* object = imageSet.QueryItem(id, &type); object && type == desiredType)
		{
			return static_cast<const T*>(object);
		}
		return nullptr;
	}
}

namespace kxf
{
	wxIMPLEMENT_DYNAMIC_CLASS(ImageSet, wxObject);

	wxImage ImageSet::GetImage(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const wxObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Image:
				{
					return *static_cast<const wxImage*>(object);
				}
				case ItemType::Bitmap:
				{
					return Drawing::ToImage(*static_cast<const wxBitmap*>(object));
				}
				case ItemType::Icon:
				{
					return Drawing::ToImage(*static_cast<const wxIcon*>(object));
				}
			};
		}
		return {};
	}
	wxBitmap ImageSet::GetBitmap(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const wxObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Image:
				{
					return Drawing::ToBitmap(*static_cast<const wxImage*>(object));
				}
				case ItemType::Bitmap:
				{
					return *static_cast<const wxBitmap*>(object);
				}
				case ItemType::Icon:
				{
					return Drawing::ToBitmap(*static_cast<const wxIcon*>(object));
				}
			};
		}
		return wxNullBitmap;
	}
	wxIcon ImageSet::GetIcon(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const wxObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Image:
				{
					return Drawing::ToIcon(*static_cast<const wxImage*>(object));
				}
				case ItemType::Bitmap:
				{
					return Drawing::ToIcon(*static_cast<const wxBitmap*>(object));
				}
				case ItemType::Icon:
				{
					return *static_cast<const wxIcon*>(object);
				}
			};
		}
		return wxNullIcon;
	}

	Size ImageSet::GetItemSize(const String& id) const
	{
		ItemType type = ItemType::None;
		if (const wxObject* object = QueryItem(id, &type))
		{
			switch (type)
			{
				case ItemType::Bitmap:
				{
					return static_cast<const wxBitmap&>(*object).GetSize();
				}
				case ItemType::Image:
				{
					return static_cast<const wxImage&>(*object).GetSize();
				}
				case ItemType::Icon:
				{
					return static_cast<const wxIcon&>(*object).GetSize();
				}
			};
		};
		return Size::UnspecifiedSize();
	}
	const wxObject* ImageSet::QueryItem(const String& id, ItemType* type) const
	{
		if (auto it = m_Items.find(id); it != m_Items.end())
		{
			const auto& var = it->second;
			switch (static_cast<ItemType>(var.index()))
			{
				case ItemType::Bitmap:
				{
					Utility::SetIfNotNull(type, ItemType::Bitmap);
					return &std::get<wxBitmap>(var);
				}
				case ItemType::Image:
				{
					Utility::SetIfNotNull(type, ItemType::Image);
					return &std::get<wxImage>(var);
				}
				case ItemType::Icon:
				{
					Utility::SetIfNotNull(type, ItemType::Icon);
					return &std::get<wxIcon>(var);
				}
			};
		}

		Utility::SetIfNotNull(type, ItemType::None);
		return nullptr;
	}
	const wxImage* ImageSet::QueryImage(const String& id) const
	{
		return GetPtr<wxImage>(*this, id, ItemType::Image);
	}
	const wxBitmap* ImageSet::QueryBitmap(const String& id) const
	{
		return GetPtr<wxBitmap>(*this, id, ItemType::Bitmap);
	}
	const wxIcon* ImageSet::QueryIcon(const String& id) const
	{
		return GetPtr<wxIcon>(*this, id, ItemType::Icon);
	}

	std::unique_ptr<ImageList> ImageSet::CreateImageList(const Size& size) const
	{
		std::unique_ptr<ImageList> imageList = std::make_unique<ImageList>(size, GetCount());

		for (const auto& item: m_Items)
		{
			ItemType type = ItemType::None;
			if (const wxObject* object = QueryItem(item.first, &type))
			{
				switch (type)
				{
					case ItemType::Image:
					{
						imageList->Add(*static_cast<const wxImage*>(object));
						break;
					}
					case ItemType::Bitmap:
					{
						imageList->Add(*static_cast<const wxBitmap*>(object));
						break;
					}
					case ItemType::Icon:
					{
						imageList->Add(*static_cast<const wxIcon*>(object));
						break;
					}
				};
			}
		}

		return imageList;
	}
}
