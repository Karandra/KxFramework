#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxImageList: public wxImageList
{
	public:
		KxImageList() {}
		KxImageList(int width, int height, bool mask = false, int initialCount = 1)
			:wxImageList(width, height, mask, initialCount)
		{
		}
		bool Create(int width, int height, bool mask = false, int initialCount = 1)
		{
			return wxImageList::Create(width, height, mask, initialCount);
		}

	public:
		int Add(const wxBitmap& bitmap)
		{
			return wxImageList::Add(bitmap, wxNullBitmap);
		}
		int Add(const wxIcon& icon)
		{
			return wxImageList::Add(icon);
		}
		int Add(const wxImage& image)
		{
			return Add(wxBitmap(image, 32));
		}
		
		bool Replace(int index, const wxBitmap& bitmap)
		{
			return wxImageList::Replace(index, bitmap, wxNullBitmap);
		}
		bool Replace(int index, const wxIcon& icon)
		{
			return wxImageList::Replace(index, icon);
		}
		bool Replace(int index, const wxImage& image)
		{
			return wxImageList::Replace(index, wxBitmap(image, 32));
		}
		
		virtual wxImage GetImage(int index) const
		{
			wxImage image = GetBitmap(index).ConvertToImage();
			if (image.HasAlpha())
			{
				image.InitAlpha();
			}
			return image;
		}
		virtual wxBitmap GetBitmap(int index) const
		{
			return wxBitmap(GetIcon(index));
		}
		virtual wxIcon GetIcon(int index) const
		{
			return wxImageList::GetIcon(index);
		}

		void Clear()
		{
			RemoveAll();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxImageList);
};
