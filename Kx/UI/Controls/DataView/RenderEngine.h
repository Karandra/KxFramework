#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "CellState.h"
#include "CellAttribute.h"

namespace KxFramework::UI::DataView
{
	class Renderer;
}

namespace KxFramework::UI::DataView
{
	enum class MarkupMode
	{
		Disabled = 0,
		TextOnly,
		WithMnemonics
	};
	enum class ProgressState
	{
		Normal,
		Paused,
		Error,
		Partial,
	};
	enum class ToggleState
	{
		None = -2,

		Checked = 1,
		Unchecked = 0,
		Indeterminate = -1,
	};
	enum class ToggleType
	{
		None = -1,

		CheckBox,
		RadioBox,
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API RenderEngine
	{
		private:
			Renderer& m_Renderer;
			bool m_AlwaysUseGC = false;

		public:
			RenderEngine(Renderer& renderer, bool alwaysUseGC = false)
				:m_Renderer(renderer), m_AlwaysUseGC(alwaysUseGC)
			{
			}

		public:
			wxDC* GetTextRenderingDC() const;

			bool IsAlwaysUseingGraphicsContext() const
			{
				return m_AlwaysUseGC;
			}
			void AlwaysUseGraphicsContext(bool alwaysGC)
			{
				m_AlwaysUseGC = alwaysGC;
			}

			int GetInterTextSpacing() const
			{
				return 2;
			}
			int CalcCenter(int cellSize, int itemSize) const;
			
			wxSize FromDIP(const wxSize& size) const;
			wxSize FromDIP(int x, int y) const
			{
				return FromDIP(wxSize(x, y));
			}
			int FromDIPX(int x) const
			{
				return FromDIP(x, wxDefaultCoord).GetWidth();
			}
			int FromDIPY(int y) const
			{
				return FromDIP(wxDefaultCoord, y).GetHeight();
			}

			size_t FindFirstLineBreak(const wxString& string) const;
			int GetControlFlags(CellState cellState) const;
			wxString StripMarkup(const wxString& markup) const;
			wxRect CenterTextInside(const wxRect& cellRect, const wxSize& textExtent) const
			{
				wxPoint pos = cellRect.GetPosition();
				pos.y += CalcCenter(cellRect.GetSize().GetHeight(), textExtent.GetHeight());

				return wxRect(pos, cellRect.GetSize());
			}

			wxSize GetTextExtent(const wxString& string) const;
			wxSize GetTextExtent(wxDC& dc, const wxString& string) const;
			
			wxSize GetMultilineTextExtent(const wxString& string) const;
			wxSize GetMultilineTextExtent(wxDC& dc, const wxString& string) const;

			bool DrawText(const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX = 0);
			bool DrawText(wxDC& dc, const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX = 0);

			bool DrawBitmap(const wxRect& cellRect, CellState cellState, const wxBitmap& bitmap, int reservedWidth = -1);
			int DrawBitmapWithText(const wxRect& cellRect, CellState cellState, int offsetX, const wxString& text, const wxBitmap& bitmap, bool centerTextV = false, int reservedWidth = -1);
			bool DrawProgressBar(const wxRect& cellRect, CellState cellState, int value, int range, ProgressState state = ProgressState::Normal, Color* averageBackgroundColor = nullptr);
			
			wxSize GetToggleSize() const;
			wxSize DrawToggle(wxDC& dc, const wxRect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType);

		public:
			static void DrawPlusMinusExpander(wxWindow* window, wxDC& dc, const wxRect& canvasRect, int flags);
			static void DrawSelectionRect(wxWindow* window, wxDC& dc, const wxRect& cellRect, int flags);;
	};
}
