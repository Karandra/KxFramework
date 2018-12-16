#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

class KX_API KxDataViewTextRenderer: public KxDataViewRenderer
{
	public:
		static bool GetValueAsString(const wxAny& value, wxString& string)
		{
			return value.GetAs<wxString>(&string);
		}

	private:
		wxString m_Text;

	protected:
		virtual bool SetValue(const wxAny& value);

		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewTextRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}
};
