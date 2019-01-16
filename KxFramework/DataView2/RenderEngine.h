#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include "Common.h"
#include "CellState.h"
#include "ItemAttributes.h"

namespace Kx::DataView2
{
	class KX_API Renderer;
}

namespace Kx::DataView2
{
	enum class ProgressBarState
	{
		Normal,
		Paused,
		Error,
		Partial,
	};
	enum class MarkupMode
	{
		Disabled = 0,
		TextOnly,
		WithMnemonics
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
			int CalcCenter(int pos, int size) const;
			int FindFirstNewLinePos(const wxString& string) const;
			int GetControlFlags(CellState cellState) const;

			wxSize GetTextExtent(const wxString& string) const;
			wxSize GetTextExtent(wxDC& dc, const wxString& string) const;

			bool DrawText(const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX = 0);
			bool DrawText(wxDC& dc, const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX = 0);

			bool DoDrawBitmap(const wxRect& cellRect, CellState cellState, const wxBitmap& bitmap);
			bool DoDrawProgressBar(const wxRect& cellRect, CellState cellState, int value, int range, ProgressBarState state = ProgressBarState::Normal);
	};
}
