#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

class wxHtmlDCRenderer;
class KxDataViewHTMLRenderer: public KxDataViewRenderer
{
	private:
		wxString m_Content;
		wxString m_ContentHTML;

		wxArrayInt m_KnownPageBreaks;
		double m_PixelScale = 1.0;
		double m_FontScale = 1.0;

	protected:
		virtual bool SetValue(const wxAny& value);

		void PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, wxDC& dc, const wxRect& cellRect = wxNullRect) const;
		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewHTMLRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}

	public:
		KxIntVector GetKnownPageBreaks() const;
		void SetKnownPageBreaks(const KxIntVector& tBreaks);

		void SetPixelScale(double v)
		{
			m_PixelScale = v;
		}
		void SetFontScale(double v)
		{
			m_FontScale = v;
		}
		void SetScale(double v)
		{
			m_FontScale = v;
			m_PixelScale = v;
		}
};
