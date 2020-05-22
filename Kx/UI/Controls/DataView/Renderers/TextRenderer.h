#pragma once
#include "../Renderer.h"

namespace KxFramework::UI::DataView
{
	class KX_API TextValue
	{
		protected:
			wxString m_Text;

		public:
			TextValue(const wxString& text = {})
				:m_Text(text)
			{
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				ClearText();
			}

			bool HasText() const
			{
				return !m_Text.IsEmpty();
			}
			const wxString& GetText() const
			{
				return m_Text;
			}
			void SetText(const wxString& text)
			{
				m_Text = text;
			}
			void ClearText()
			{
				m_Text.clear();
			}
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API TextRenderer: public Renderer
	{
		private:
			TextValue m_Value;

		protected:
			bool SetValue(const wxAny& value) override;
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}

			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			TextRenderer(int alignment = wxALIGN_INVALID)
				:Renderer(alignment)
			{
			}
			
		public:
			wxString GetTextValue(const wxAny& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
