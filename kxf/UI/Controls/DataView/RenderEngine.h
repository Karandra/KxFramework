#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttribute.h"
#include "kxf/Drawing/GDICanvas.h"
#include "kxf/Drawing/GDIGraphicsCanvas.h"
#include "kxf/Drawing/Bitmap.h"
#include "kxf/Drawing/Icon.h"

namespace kxf::UI::DataView
{
	class Renderer;
}

namespace kxf::UI::DataView
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

namespace kxf::UI::DataView
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
			GDICanvas GetTextRenderingDC() const;

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

			Size FromDIP(const Size& size) const;
			Size FromDIP(int x, int y) const
			{
				return FromDIP(Size(x, y));
			}
			int FromDIPX(int x) const
			{
				return FromDIP(x, wxDefaultCoord).GetWidth();
			}
			int FromDIPY(int y) const
			{
				return FromDIP(wxDefaultCoord, y).GetHeight();
			}

			size_t FindFirstLineBreak(const String& string) const;
			int GetControlFlags(CellState cellState) const;
			String StripMarkup(const String& markup) const;
			Rect CenterTextInside(const Rect& cellRect, const Size& textExtent) const
			{
				Point pos = cellRect.GetPosition();
				pos.Y() += CalcCenter(cellRect.GetSize().GetHeight(), textExtent.GetHeight());

				return Rect(pos, cellRect.GetSize());
			}

			Size GetTextExtent(const String& string) const;
			Size GetTextExtent(GDICanvas& dc, const String& string) const;

			Size GetMultilineTextExtent(const String& string) const;
			Size GetMultilineTextExtent(GDICanvas& dc, const String& string) const;

			bool DrawText(const Rect& cellRect, CellState cellState, const String& string, int offsetX = 0);
			bool DrawText(GDICanvas& dc, const Rect& cellRect, CellState cellState, const String& string, int offsetX = 0);

			bool DrawBitmap(const Rect& cellRect, CellState cellState, const Bitmap& bitmap, int reservedWidth = -1);
			int DrawBitmapWithText(const Rect& cellRect, CellState cellState, int offsetX, const String& text, const Bitmap& bitmap, bool centerTextV = false, int reservedWidth = -1);
			bool DrawProgressBar(const Rect& cellRect, CellState cellState, int value, int range, ProgressState state = ProgressState::Normal, Color* averageBackgroundColor = nullptr);

			Size GetToggleSize() const;
			Size DrawToggle(GDICanvas& dc, const Rect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType);

		public:
			static void DrawPlusMinusExpander(wxWindow* window, GDICanvas& dc, const Rect& canvasRect, int flags);
			static void DrawSelectionRect(wxWindow* window, GDICanvas& dc, const Rect& cellRect, int flags);;
	};
}
