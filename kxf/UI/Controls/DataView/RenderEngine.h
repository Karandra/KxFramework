#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttribute.h"
#include "kxf/Drawing/GDIRenderer/GDIContext.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/Drawing/GDIRenderer/GDIAction.h"
#include "kxf/Drawing/GraphicsRenderer.h"

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

		public:
			RenderEngine(Renderer& renderer)
				:m_Renderer(renderer)
			{
			}

		public:
			int GetInterTextSpacing() const
			{
				return 2;
			}
			int CalcCenter(int cellSize, int itemSize) const;

			Size FromDIP(const Size& size) const;
			Size FromDIP(int x, int y) const
			{
				return FromDIP({x, y});
			}
			int FromDIPX(int x) const
			{
				return FromDIP(x, Geometry::DefaultCoord).GetWidth();
			}
			int FromDIPY(int y) const
			{
				return FromDIP(Geometry::DefaultCoord, y).GetHeight();
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
			Size GetTextExtent(GDIContext& dc, const String& string) const;

			Size GetMultilineTextExtent(const String& string) const;
			Size GetMultilineTextExtent(GDIContext& dc, const String& string) const;

			bool DrawText(const Rect& cellRect, CellState cellState, const String& string, int offsetX = 0);
			bool DrawText(GDIContext& dc, const Rect& cellRect, CellState cellState, const String& string, int offsetX = 0);

			bool DrawBitmap(const Rect& cellRect, CellState cellState, const GDIBitmap& bitmap, int reservedWidth = -1);
			int DrawBitmapWithText(const Rect& cellRect, CellState cellState, int offsetX, const String& text, const GDIBitmap& bitmap, bool centerTextV = false, int reservedWidth = -1);
			bool DrawProgressBar(const Rect& cellRect, CellState cellState, int value, int range, ProgressState state = ProgressState::Normal, Color* averageBackgroundColor = nullptr);

			Size GetToggleSize() const;
			Size DrawToggle(GDIContext& dc, const Rect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType);

		public:
			static void DrawPlusMinusExpander(wxWindow* window, GDIContext& dc, const Rect& canvasRect, int flags);
			static void DrawSelectionRect(wxWindow* window, GDIContext& dc, const Rect& cellRect, int flags);;
	};
}
