#include "KxStdAfx.h"
#include "KxFramework/KxImageSet.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxImageSet, wxObject);

KxImageSet::KxImageSet(size_t count)
{
	m_Data.reserve(count);
}
KxImageSet::~KxImageSet()
{
}

void KxImageSet::Set(const wxString& id, const wxImage& image)
{
	m_Data.insert_or_assign(id, image);
}
void KxImageSet::Set(const wxString& id, const wxBitmap& image)
{
	wxImage tempImage = image.ConvertToImage();
	if (tempImage.HasAlpha())
	{
		tempImage.InitAlpha();
		m_Data.insert_or_assign(id, wxBitmap(tempImage, 32));
	}
	else
	{
		m_Data.insert_or_assign(id, image);
	}
}
void KxImageSet::Set(const wxString& id, const wxIcon& image)
{
	m_Data.insert_or_assign(id, image);
}
const wxObject* KxImageSet::Get(const wxString& id, ImageType* type) const
{
	auto it = m_Data.find(id);
	if (it != m_Data.end())
	{
		auto& var = it->second;
		switch (var.index())
		{
			case TYPE_BITMAP:
			{
				KxUtility::SetIfNotNull(type, TYPE_BITMAP);
				return &std::get<wxBitmap>(var);
				break;
			}
			case TYPE_IMAGE:
			{
				KxUtility::SetIfNotNull(type, TYPE_IMAGE);
				return &std::get<wxImage>(var);
				break;
			}
			case TYPE_ICON:
			{
				KxUtility::SetIfNotNull(type, TYPE_ICON);
				return &std::get<wxIcon>(var);
				break;
			}
		};
	}

	KxUtility::SetIfNotNull(type, TYPE_INVALID);
	return nullptr;
}
void KxImageSet::Remove(const wxString& id)
{
	m_Data.erase(id);
}

wxImage KxImageSet::GetImage(const wxString& id) const
{
	ImageType type = TYPE_INVALID;
	const wxObject* object = Get(id, &type);

	if (object)
	{
		switch (type)
		{
			case TYPE_IMAGE:
			{
				return *static_cast<const wxImage*>(object);
			}
			case TYPE_BITMAP:
			{
				return static_cast<const wxBitmap*>(object)->ConvertToImage();
			}
			case TYPE_ICON:
			{
				wxBitmap bitmap(*static_cast<const wxIcon*>(object));
				return bitmap.ConvertToImage();
			}
		};
	}
	return wxNullImage;
}
wxBitmap KxImageSet::GetBitmap(const wxString& id) const
{
	ImageType type = TYPE_INVALID;
	const wxObject* object = Get(id, &type);

	if (object)
	{
		switch (type)
		{
			case TYPE_IMAGE:
			{
				return wxBitmap(*static_cast<const wxImage*>(object), 32);
			}
			case TYPE_BITMAP:
			{
				return *static_cast<const wxBitmap*>(object);
			}
			case TYPE_ICON:
			{
				return wxBitmap(*static_cast<const wxIcon*>(object));
			}
		};
	}
	return wxNullBitmap;
}
wxIcon KxImageSet::GetIcon(const wxString& id) const
{
	ImageType type = TYPE_INVALID;
	const wxObject* object = Get(id, &type);

	if (object)
	{
		switch (type)
		{
			case TYPE_IMAGE:
			{
				wxBitmap bitmap(*static_cast<const wxImage*>(object));
				wxIcon icon;
				icon.CopyFromBitmap(bitmap);
				return icon;
			}
			case TYPE_BITMAP:
			{
				wxIcon icon;
				icon.CopyFromBitmap(*static_cast<const wxBitmap*>(object));
				return icon;
			}
			case TYPE_ICON:
			{
				return *static_cast<const wxIcon*>(object);
			}
		};
	}
	return wxNullIcon;
}
wxImageList* KxImageSet::ToImageList(const wxSize& size) const
{
	wxImageList* imageList = new wxImageList(size.GetWidth(), size.GetHeight(), false, GetCount());

	for (auto& i: m_Data)
	{
		wxString id = i.first;
		ImageType type = TYPE_INVALID;
		const wxObject* object = Get(id, &type);

		if (object)
		{
			switch (type)
			{
				case TYPE_IMAGE:
				{
					imageList->Add(*static_cast<const wxImage*>(object));
					break;
				}
				case TYPE_BITMAP:
				{
					imageList->Add(*static_cast<const wxBitmap*>(object));
					break;
				}
				case TYPE_ICON:
				{
					imageList->Add(*static_cast<const wxIcon*>(object));
					break;
				}
			};
		}
	}

	return imageList;
}
