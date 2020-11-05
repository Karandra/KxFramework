#include "stdafx.h"
#include "ImageSet.h"
#include "ImageList.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	Image ImageSet::GetImage(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			if (auto image = object->QueryInterface<Image>())
			{
				return *image;
			}
			else if (auto bitmap = object->QueryInterface<Bitmap>())
			{
				return bitmap->ToImage();
			}
			else if (auto icon = object->QueryInterface<Icon>())
			{
				return icon->ToImage();
			}
		}
		return {};
	}
	Bitmap ImageSet::GetBitmap(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			if (auto image = object->QueryInterface<Image>())
			{
				return image->ToBitmap();
			}
			else if (auto bitmap = object->QueryInterface<Bitmap>())
			{
				return *bitmap;
			}
			else if (auto icon = object->QueryInterface<Icon>())
			{
				return icon->ToBitmap();
			}
		}
		return {};
	}
	Icon ImageSet::GetIcon(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			if (auto image = object->QueryInterface<Image>())
			{
				return image->ToIcon();
			}
			else if (auto bitmap = object->QueryInterface<Bitmap>())
			{
				return bitmap->ToIcon();
			}
			else if (auto icon = object->QueryInterface<Icon>())
			{
				return *icon;
			}
		}
		return {};
	}

	Size ImageSet::GetItemSize(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			if (auto image = object->QueryInterface<Image>())
			{
				return image->GetSize();
			}
			else if (auto gdiImage = object->QueryInterface<IGDIImage>())
			{
				return gdiImage->GetSize();
			}
		}
		return Size::UnspecifiedSize();
	}
	const IObject* ImageSet::QueryItem(const String& id) const
	{
		if (auto it = m_Items.find(id); it != m_Items.end())
		{
			const auto& item = it->second;
			if (auto bitmap = std::get_if<Bitmap>(&item))
			{
				return bitmap;
			}
			else if (auto image = std::get_if<Image>(&item))
			{
				return image;
			}
			else if (auto icon = std::get_if<Icon>(&item))
			{
				return icon;
			}
		}
		return nullptr;
	}
	const Image* ImageSet::QueryImage(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			return object->QueryInterface<Image>().get();
		}
		return nullptr;
	}
	const Bitmap* ImageSet::QueryBitmap(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			return object->QueryInterface<Bitmap>().get();
		}
		return nullptr;
	}
	const Icon* ImageSet::QueryIcon(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			return object->QueryInterface<Icon>().get();
		}
		return nullptr;
	}

	std::unique_ptr<ImageList> ImageSet::CreateImageList(const Size& size) const
	{
		std::unique_ptr<ImageList> imageList = std::make_unique<ImageList>(size, GetCount());

		for (const auto& [id, item]: m_Items)
		{
			if (const IObject* object = QueryItem(id))
			{
				if (auto image = object->QueryInterface<Image>())
				{
					imageList->Add(*image);
				}
				else if (auto bitmap = object->QueryInterface<Bitmap>())
				{
					imageList->Add(*bitmap);
				}
				else if (auto icon = object->QueryInterface<Icon>())
				{
					imageList->Add(*icon);
				}
			}
		}
		return imageList;
	}
}
