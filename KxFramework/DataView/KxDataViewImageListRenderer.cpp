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
	using namespace KxEnumClassOperations;

	if (KxImageList* imageList = GetImageList())
	{
		const int imageWidth = imageList->GetSize().GetWidth();
		KxImageListDrawMode drawMode = KxImageListDrawMode::Normal|KxImageListDrawMode::Transparent;
		if (IsOptionEnabled(KxDVR_IMAGELIST_BITMAP_DRAWING))
		{
			if (cellState & KxDATAVIEW_CELL_SELECTED)
			{
				drawMode |= KxImageListDrawMode::Selected;
			}
			if (cellState & KxDATAVIEW_CELL_HIGHLIGHTED)
			{
				drawMode |= KxImageListDrawMode::Focused;
			}
		}

		int index = 0;
		for (size_t i = 0; i < GetActualImageCount(); i++)
		{
			const int imageID = GetImageID(i);
			if (imageID != NO_IMAGE)
			{
				const int x = cellRect.GetX() + (index * (imageWidth + m_Spacing));
				const int y = cellRect.GetY();

				// Don't draw images with invalid indexes, but count them as drawn to allow spaces.
				if (imageID >= 0)
				{
					imageList->Draw(GetDC(), imageID, wxRect(wxPoint(x, y), cellRect.GetSize()), drawMode);
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
