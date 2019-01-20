#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include "Common.h"
#include "CellState.h"
#include "CellAttributes.h"

namespace Kx::DataView2
{
	class KX_API Renderer;
}

namespace Kx::DataView2
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

namespace Kx::DataView2
{
	class KX_API RenderEngine
	{
		private:
			Renderer& m_Renderer;

		public:
			RenderEngine(Renderer& renderer)
				:m_Renderer(renderer)
			{
			}

		public:
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

			wxSize GetTextExtent(const wxString& string) const;
			wxSize GetTextExtent(wxDC& dc, const wxString& string) const;

			bool DrawText(const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX = 0);
			bool DrawText(wxDC& dc, const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX = 0);

			bool DrawBitmap(const wxRect& cellRect, CellState cellState, const wxBitmap& bitmap);
			int DrawBitmapWithText(const wxRect& cellRect, CellState cellState, int offsetX, const wxString& text, const wxBitmap& bitmap, bool centerTextV = false);
			bool DrawProgressBar(const wxRect& cellRect, CellState cellState, int value, int range, ProgressState state = ProgressState::Normal);
			
			wxSize GetToggleSize() const;
			wxSize DrawToggle(wxDC& dc, const wxRect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType);
	};
}
