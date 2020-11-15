#include "stdafx.h"
#include "ImageSet.h"
#include "GDIRenderer/GDIImageList.h"
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
			else if (auto bitmap = object->QueryInterface<GDIBitmap>())
			{
				return bitmap->ToImage();
			}
			else if (auto icon = object->QueryInterface<GDIIcon>())
			{
				return icon->ToImage();
			}
		}
		return {};
	}
	GDIBitmap ImageSet::GetBitmap(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			if (auto image = object->QueryInterface<Image>())
			{
				return image->ToBitmap();
			}
			else if (auto bitmap = object->QueryInterface<GDIBitmap>())
			{
				return *bitmap;
			}
			else if (auto icon = object->QueryInterface<GDIIcon>())
			{
				return icon->ToBitmap();
			}
		}
		return {};
	}
	GDIIcon ImageSet::GetIcon(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			if (auto image = object->QueryInterface<Image>())
			{
				return image->ToIcon();
			}
			else if (auto bitmap = object->QueryInterface<GDIBitmap>())
			{
				return bitmap->ToIcon();
			}
			else if (auto icon = object->QueryInterface<GDIIcon>())
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
			if (auto bitmap = std::get_if<GDIBitmap>(&item))
			{
				return bitmap;
			}
			else if (auto image = std::get_if<Image>(&item))
			{
				return image;
			}
			else if (auto icon = std::get_if<GDIIcon>(&item))
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
	const GDIBitmap* ImageSet::QueryBitmap(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			return object->QueryInterface<GDIBitmap>().get();
		}
		return nullptr;
	}
	const GDIIcon* ImageSet::QueryIcon(const String& id) const
	{
		if (const IObject* object = QueryItem(id))
		{
			return object->QueryInterface<GDIIcon>().get();
		}
		return nullptr;
	}

	std::unique_ptr<GDIImageList> ImageSet::CreateImageList(const Size& size) const
	{
		std::unique_ptr<GDIImageList> imageList = std::make_unique<GDIImageList>(size, GetCount());

		for (const auto& [id, item]: m_Items)
		{
			if (const IObject* object = QueryItem(id))
			{
				if (auto image = object->QueryInterface<Image>())
				{
					imageList->Add(*image);
				}
				else if (auto bitmap = object->QueryInterface<GDIBitmap>())
				{
					imageList->Add(*bitmap);
				}
				else if (auto icon = object->QueryInterface<GDIIcon>())
				{
					imageList->Add(*icon);
				}
			}
		}
		return imageList;
	}
}
