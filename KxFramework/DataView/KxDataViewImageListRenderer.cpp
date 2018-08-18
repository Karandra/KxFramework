#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewImageListRenderer.h"

size_t KxDataViewImageListRendererBase::GetActualImageCount() const
{
	if (HasImageList())
	{
		return std::min((size_t)GetImageList()->GetImageCount(), GetImageCount());
	}
	return 0;
}
size_t KxDataViewImageListRendererBase::GetEffectiveImageCount() const
{
	int count = 0;
	for (size_t i = 0; i < GetActualImageCount(); i++)
	{
		if (GetImageID(i) != NO_IMAGE)
		{
			count++;
		}
	}
	return count;
}

void KxDataViewImageListRendererBase::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	if (KxImageList* imageList = GetImageList())
	{
		int drawFlags = wxIMAGELIST_DRAW_NORMAL|wxIMAGELIST_DRAW_TRANSPARENT;
		if (IsOptionEnabled(KxDVR_IMAGELIST_BITMAP_DRAWING))
		{
			if (cellState & KxDATAVIEW_CELL_SELECTED)
			{
				drawFlags |= wxIMAGELIST_DRAW_SELECTED;
			}
			if (cellState & KxDATAVIEW_CELL_HIGHLIGHTED)
			{
				drawFlags |= wxIMAGELIST_DRAW_FOCUSED;
			}
		}

		int imageWidth = imageList->GetSize().GetWidth();
		bool hasSolidBackground = HasSolidBackground();

		int index = 0;
		for (size_t i = 0; i < GetActualImageCount(); i++)
		{
			int imageID = GetImageID(i);
			if (imageID != NO_IMAGE)
			{
				int x = cellRect.GetX() + (index * (imageWidth + m_Spacing));
				int y = cellRect.GetY();

				// Don't draw images with invalid indexes, but count them as drawn to allow spaces.
				if (imageID >= 0)
				{
					imageList->Draw(imageID, GetDC(), x, y, drawFlags, hasSolidBackground);
				}
				index++;
			}
		}
	}
}
wxSize KxDataViewImageListRendererBase::GetCellSize() const
{
	if (const KxImageList* imageList = GetImageList())
	{
		wxSize size = GetImageList()->GetSize();
		size.x = (size.x + m_Spacing) * GetEffectiveImageCount();
		return size;
	}
	return KxDataViewRenderer::GetCellSize();
}

void KxDataViewImageListRendererBase::SetSpacing(int value)
{
	wxWindow* window = GetView();
	if (!window)
	{
		window = wxTheApp->GetTopWindow();
	}
	m_Spacing = window->FromDIP(value);
}
