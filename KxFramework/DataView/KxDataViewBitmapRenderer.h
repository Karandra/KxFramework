#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

class KX_API KxDataViewBitmapRenderer: public KxDataViewRenderer
{
	public:
		static bool GetValueAsBitmap(const wxAny& value, wxBitmap& bitmap);

	private:
		wxBitmap m_Bitmap;

	protected:
		virtual bool SetValue(const wxAny& value);
		
		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewBitmapRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}
};
