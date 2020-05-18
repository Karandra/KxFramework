#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
class wxHtmlDCRenderer;

namespace KxFramework::UI::DataView
{
	class KX_API HTMLRenderer: public Renderer
	{
		private:
			TextValue m_Value;
			wxString m_ContentHTML;

			double m_PixelScale = 1.0;
			double m_FontScale = 1.0;

			int m_VisibleCellFrom = 0;
			int m_VisibleCellTo = std::numeric_limits<int>::max();

		protected:
			bool SetValue(const wxAny& value) override;
			ToolTip CreateToolTip() const override;

			void PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, wxDC& dc, const wxRect& cellRect = {}) const;
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			HTMLRenderer(int alignment = wxALIGN_INVALID);
			~HTMLRenderer();

		public:
			wxString GetTextValue(const wxAny& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}

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
}
