#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewBitmapTextRenderer.h"
#include "KxFramework/DataView/KxDataViewToggleRenderer.h"

class KX_API KxDataViewBitmapTextToggleValue:
	public KxDataViewBitmapTextValue,
	public KxDataViewToggleValue
{
	public:
		using ToggleState = KxDataViewToggleValue::ToggleState;
		using ToggleType = KxDataViewToggleValue::ToggleType;

	public:
		KxDataViewBitmapTextToggleValue(const wxString& text = wxEmptyString,
										const wxBitmap& bitmap = wxNullBitmap,
										ToggleState state = InvalidState,
										ToggleType type = InvalidType
		)
			:KxDataViewBitmapTextValue(text, bitmap), KxDataViewToggleValue(state, type)
		{
		}

		KxDataViewBitmapTextToggleValue(const wxBitmap& bitmap)
			:KxDataViewBitmapTextToggleValue(wxEmptyString, bitmap)
		{
		}
		KxDataViewBitmapTextToggleValue(ToggleState state, ToggleType type = InvalidType)
			:KxDataViewBitmapTextToggleValue(wxEmptyString, wxNullBitmap, state, type)
		{
		}
		KxDataViewBitmapTextToggleValue(bool checked, ToggleType type = InvalidType)
			:KxDataViewBitmapTextToggleValue(wxEmptyString, wxNullBitmap, InvalidState, type)
		{
			SetChecked(checked);
		}
		KxDataViewBitmapTextToggleValue(bool checked, const wxString& text, const wxBitmap& bitmap = wxNullBitmap, ToggleType type = InvalidType)
			:KxDataViewBitmapTextToggleValue(text, bitmap, InvalidState, type)
		{
			SetChecked(checked);
		}
};

class KX_API KxDataViewBitmapTextToggleRenderer: public KxDataViewRenderer, public KxDataViewToggleRendererBase
{
	private:
		KxDataViewBitmapTextToggleValue m_Value;

	protected:
		virtual bool HasActivator() const override
		{
			return true;
		}
		virtual bool OnActivateCell(const KxDataViewItem& item, const wxRect& cellRect, const wxMouseEvent* mouseEvent = NULL);

		virtual bool IsEnabled() const override;
		virtual bool SetValue(const wxAny& value);

		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewBitmapTextToggleRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}
};
