#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
class wxHtmlDCRenderer;

namespace kxf::UI::DataView
{
	class KX_API HTMLRenderer: public Renderer
	{
		private:
			TextValue m_Value;
			String m_ContentHTML;

			double m_PixelScale = 1.0;
			double m_FontScale = 1.0;

			int m_VisibleCellFrom = 0;
			int m_VisibleCellTo = std::numeric_limits<int>::max();

		protected:
			bool SetValue(const wxAny& value) override;
			ToolTip CreateToolTip() const override;

			void PrepareRenderer(wxHtmlDCRenderer& htmlRenderer, GDICanvas& dc, const Rect& cellRect = {}) const;
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			HTMLRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment)
			{
			}

		public:
			String GetTextValue(const wxAny& value) const override
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
