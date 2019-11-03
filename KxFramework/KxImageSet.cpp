#include "KxStdAfx.h"
#include "KxFramework/KxImageSet.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxImageSet, wxObject);

namespace
{
	template<class T>
	const T* GetPtr(const KxImageSet& imageSet, const wxString& id, KxImageSet::ImageType desiredType)
	{
		using ImageType = KxImageSet::ImageType;

		ImageType type = ImageType::Invalid;
		if (const wxObject* object = imageSet.Get(id, &type); object && type == desiredType)
		{
			return static_cast<const T*>(object);
		}
		return nullptr;
	}
}

KxImageSet::KxImageSet(size_t initialCount)
{
	m_Store.reserve(initialCount);
}

bool KxImageSet::Remove(const wxString& id)
{
	return m_Store.erase(id) != 0;
}
void KxImageSet::Clear()
{
	m_Store.clear();
}

void KxImageSet::Set(const wxString& id, const wxImage& image)
{
	m_Store.insert_or_assign(id, image);
}
void KxImageSet::Set(const wxString& id, const wxBitmap& image)
{
	wxImage tempImage = image.ConvertToImage();
	if (tempImage.HasAlpha())
	{
		tempImage.InitAlpha();
		m_Store.insert_or_assign(id, wxBitmap(tempImage, 32));
	}
	else
	{
		m_Store.insert_or_assign(id, image);
	}
}
void KxImageSet::Set(const wxString& id, const wxIcon& image)
{
	m_Store.insert_or_assign(id, image);
}

const wxObject* KxImageSet::Get(const wxString& id, ImageType* type) const
{
	auto it = m_Store.find(id);
	if (it != m_Store.end())
	{
		const auto& var = it->second;
		switch (static_cast<ImageType>(var.index()))
		{
			case ImageType::Bitmap:
			{
				KxUtility::SetIfNotNull(type, ImageType::Bitmap);
				return &std::get<wxBitmap>(var);
				break;
			}
			case ImageType::Image:
			{
				KxUtility::SetIfNotNull(type, ImageType::Image);
				return &std::get<wxImage>(var);
				break;
			}
			case ImageType::Icon:
			{
				KxUtility::SetIfNotNull(type, ImageType::Icon);
				return &std::get<wxIcon>(var);
				break;
			}
		};
	}

	KxUtility::SetIfNotNull(type, ImageType::Invalid);
	return nullptr;
}
wxSize KxImageSet::GetSize(const wxString& id) const
{
	ImageType type = ImageType::Invalid;
	if (const wxObject* object = Get(id, &type))
	{
		switch (type)
		{
			case ImageType::Bitmap:
			{
				return static_cast<const wxBitmap&>(*object).GetSize();
			}
			case ImageType::Image:
			{
				return static_cast<const wxImage&>(*object).GetSize();
			}
			case ImageType::Icon:
			{
				return static_cast<const wxIcon&>(*object).GetSize();
			}
		};
	};
	return wxDefaultSize;
}

wxImage KxImageSet::GetImage(const wxString& id) const
{
	ImageType type = ImageType::Invalid;
	if (const wxObject* object = Get(id, &type))
	{
		switch (type)
		{
			case ImageType::Image:
			{
				return *static_cast<const wxImage*>(object);
			}
			case ImageType::Bitmap:
			{
				return static_cast<const wxBitmap*>(object)->ConvertToImage();
			}
			case ImageType::Icon:
			{
				wxBitmap bitmap(*static_cast<const wxIcon*>(object), wxBitmapTransparency::wxBitmapTransparency_Auto);
				return bitmap.ConvertToImage();
			}
		};
	}
	return wxNullImage;
}
wxBitmap KxImageSet::GetBitmap(const wxString& id) const
{
	ImageType type = ImageType::Invalid;
	if (const wxObject* object = Get(id, &type))
	{
		switch (type)
		{
			case ImageType::Image:
			{
				return wxBitmap(*static_cast<const wxImage*>(object), 32);
			}
			case ImageType::Bitmap:
			{
				return *static_cast<const wxBitmap*>(object);
			}
			case ImageType::Icon:
			{
				return wxBitmap(*static_cast<const wxIcon*>(object), wxBitmapTransparency::wxBitmapTransparency_Auto);
			}
		};
	}
	return wxNullBitmap;
}
wxIcon KxImageSet::GetIcon(const wxString& id) const
{
	ImageType type = ImageType::Invalid;
	if (const wxObject* object = Get(id, &type))
	{
		switch (type)
		{
			case ImageType::Image:
			{
				wxBitmap bitmap(*static_cast<const wxImage*>(object), 32);
				wxIcon icon;
				icon.CopyFromBitmap(bitmap);
				return icon;
			}
			case ImageType::Bitmap:
			{
				wxIcon icon;
				icon.CopyFromBitmap(*static_cast<const wxBitmap*>(object));
				return icon;
			}
			case ImageType::Icon:
			{
				return *static_cast<const wxIcon*>(object);
			}
		};
	}
	return wxNullIcon;
}

const wxImage* KxImageSet::GetImagePtr(const wxString& id) const
{
	return GetPtr<wxImage>(*this, id, ImageType::Image);
}
const wxBitmap* KxImageSet::GetBitmapPtr(const wxString& id) const
{
	return GetPtr<wxBitmap>(*this, id, ImageType::Bitmap);
}
const wxIcon* KxImageSet::GetIconPtr(const wxString& id) const
{
	return GetPtr<wxIcon>(*this, id, ImageType::Icon);
}

std::unique_ptr<KxImageList> KxImageSet::CreateImageList(const wxSize& size) const
{
	std::unique_ptr<KxImageList> imageList = std::make_unique<KxImageList>(size, GetCount());

	for (const auto& item: m_Store)
	{
		ImageType type = ImageType::Invalid;
		if (const wxObject* object = Get(item.first, &type))
		{
			switch (type)
			{
				case ImageType::Image:
				{
					imageList->Add(*static_cast<const wxImage*>(object));
					break;
				}
				case ImageType::Bitmap:
				{
					imageList->Add(*static_cast<const wxBitmap*>(object));
					break;
				}
				case ImageType::Icon:
				{
					imageList->Add(*static_cast<const wxIcon*>(object));
					break;
				}
			};
		}
	}

	return imageList;
}
