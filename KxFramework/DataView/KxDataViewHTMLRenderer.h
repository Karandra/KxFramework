#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

class wxHtmlDCRenderer;
class KX_API KxDataViewHTMLRenderer: public KxDataViewRenderer
{
	private:
		wxString m_Content;
		wxString m_ContentHTML;

		double m_PixelScale = 1.0;
		double m_FontScale = 1.0;

		int m_VisibleCellFrom = 0;
		int m_VisibleCellTo = std::numeric_limits<int>::max();

	protected:
		virtual bool SetValue(const wxAny& value);

		void PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, wxDC& dc, const wxRect& cellRect = KxNullWxRect) const;
		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewHTMLRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}

	public:
		int GetVisibleCellFrom() const
		{
			return m_VisibleCellFrom;
		}
		int GetVisibleCellTo() const
		{
			return m_VisibleCellTo;
		}
		void SetVisibleCellBounds(int from = 0, int to = std::numeric_limits<int>::max())
		{
			m_VisibleCellFrom = from;
			m_VisibleCellTo = to;
		}

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
