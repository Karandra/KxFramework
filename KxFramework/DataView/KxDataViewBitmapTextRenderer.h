#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

class KxDataViewBitmapTextValue
{
	private:
		wxBitmap m_Bitmap;
		wxString m_Text;
		bool m_VCenterText = false;

	public:
		KxDataViewBitmapTextValue(const wxString& text = wxEmptyString, const wxBitmap& bitmap = wxNullBitmap)
			:m_Text(text), m_Bitmap(bitmap)
		{
		}

	public:
		bool HasText() const
		{
			return !m_Text.IsEmpty();
		}
		wxString& GetText()
		{
			return m_Text;
		}
		const wxString& GetText() const
		{
			return m_Text;
		}
		void SetText(const wxString& text)
		{
			m_Text = text;
		}

		bool HasBitmap() const
		{
			return m_Bitmap.IsOk();
		}
		wxBitmap& GetBitmap()
		{
			return m_Bitmap;
		}
		const wxBitmap& GetBitmap() const
		{
			return m_Bitmap;
		}
		void SetBitmap(const wxBitmap& bitmap)
		{
			m_Bitmap = bitmap;
		}

		void SetVCenterText(bool value)
		{
			m_VCenterText = value;
		}
		bool ShouldVCenterText() const
		{
			return m_VCenterText;
		}
};

class KxDataViewBitmapTextRenderer: public KxDataViewRenderer
{
	public:
		static wxSize GetBitmapMargins(wxWindow* window);
		static int DrawBitmapWithText(KxDataViewRenderer* rederer, const wxRect& cellRect, KxDataViewCellState cellState, int offsetX, KxDataViewBitmapTextValue& value);

	private:
		KxDataViewBitmapTextValue m_Value;

	protected:
		virtual bool SetValue(const wxAny& value);

		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewBitmapTextRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}
};
