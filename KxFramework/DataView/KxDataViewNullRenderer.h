#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

class KX_API KxDataViewNullRenderer: public KxDataViewRenderer
{
	protected:
		virtual bool SetValue(const wxAny& value)
		{
			return false;
		}

		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override
		{
		}
		virtual wxSize GetCellSize() const override
		{
			return KxDataViewRenderer::GetCellSize();
		}

	public:
		KxDataViewNullRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}
};
