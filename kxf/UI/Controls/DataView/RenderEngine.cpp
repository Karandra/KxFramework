#include "stdafx.h"
#include "RenderEngine.h"
#include "Renderer.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "kxf/Drawing/UxTheme.h"
#include "kxf/Drawing/Private/UxThemeDefines.h"
#include "kxf/Drawing/GDIRenderer.h"
#include "kxf/Drawing/GraphicsRenderer/GCOperations.h"
#include "wx/generic/private/markuptext.h"

namespace kxf::UI::DataView
{
	static bool operator<(const Size& left, const Size& right)
	{
		return left.GetWidth() < right.GetWidth() || left.GetHeight() < right.GetHeight();
	}
	static bool operator>(const Size& left, const Size& right)
	{
		return left.GetWidth() > right.GetWidth() || left.GetHeight() > right.GetHeight();
	}

	static int GetTreeItemState(int flags)
	{
		int itemState = (flags & wxCONTROL_CURRENT) ? TREIS_HOT : TREIS_NORMAL;
		if (flags & wxCONTROL_SELECTED)
		{
			itemState = (flags & wxCONTROL_CURRENT) ? TREIS_HOTSELECTED : TREIS_SELECTED;
			if (!(flags & wxCONTROL_FOCUSED))
			{
				itemState = TREIS_SELECTEDNOTFOCUS;
			}
		}

		if (flags & wxCONTROL_DISABLED && !(flags & wxCONTROL_CURRENT))
		{
			itemState = TREIS_DISABLED;
		}
		return itemState;
	};
}

namespace kxf::UI::DataView::Markup
{
	class TextOnly final: public wxItemMarkupText
	{
		public:
			TextOnly(const String& markup)
				:wxItemMarkupText(markup)
			{
			}

		public:
			Size GetTextExtent(GDIContext& dc) const
			{
				return Measure(dc.ToWxDC());
			}
	};
	class WithMnemonics final: public wxMarkupText
	{
		public:
			WithMnemonics(const String& markup)
				:wxMarkupText(markup)
			{
			}

		public:
			Size GetTextExtent(GDIContext& dc) const
			{
				return Measure(dc.ToWxDC());
			}
	};
}

namespace kxf::UI::DataView::Markup
{
	using MarkupMode = Renderer::MarkupMode;
	template<MarkupMode t_Mode> auto Create(const String& string = {})
	{
		if constexpr(t_Mode == MarkupMode::TextOnly)
		{
			return TextOnly(string);
		}
		else if constexpr(t_Mode == MarkupMode::WithMnemonics)
		{
			return WithMnemonics(string);
		}
	}

	Size GetTextExtent(const wxMarkupTextBase& markup, GDIContext& dc)
	{
		return markup.Measure(dc.ToWxDC());
	}
	Size GetTextExtent(MarkupMode mode, GDIContext& dc, const String& string)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				return Create<MarkupMode::TextOnly>(string).GetTextExtent(dc);
			}
			case MarkupMode::WithMnemonics:
			{
				return Create<MarkupMode::WithMnemonics>(string).GetTextExtent(dc);
			}
		};
		return Size(0, 0);
	}

	template<class T>
	void DrawText(T& markup, wxWindow* window, GDIContext& dc, const Rect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		if constexpr(std::is_same_v<T, WithMnemonics>)
		{
			markup.Render(dc.ToWxDC(), rect, flags);
		}
		else
		{
			markup.Render(window, dc.ToWxDC(), rect, flags, ellipsizeMode);
		}
	}

	void DrawText(MarkupMode mode, const String& string, wxWindow* window, GDIContext& dc, const Rect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				auto markup = Create<MarkupMode::TextOnly>(string);
				return DrawText(markup, window, dc, rect, flags, ellipsizeMode);
			}
			case MarkupMode::WithMnemonics:
			{
				auto markup = Create<MarkupMode::WithMnemonics>(string);
				return DrawText(markup, window, dc, rect, flags, ellipsizeMode);
			}
		};
	}
}

namespace kxf::UI::DataView
{
	GDIContext RenderEngine::GetTextRenderingDC() const
	{
		if (m_Renderer.HasRegularDC() && !m_AlwaysUseGC)
		{
			return m_Renderer.GetRegularDC().ToWxDC();
		}
		else if (m_Renderer.HasGraphicsDC())
		{
			return m_Renderer.GetGraphicsDC().ToWxDC();
		}
		return {};
	}

	int RenderEngine::CalcCenter(int cellSize, int itemSize) const
	{
		const int margins = cellSize - itemSize;

		// Can't offset by fractional values, so return 1.
		if (margins > 0 && margins <= 3)
		{
			return 1;
		}
		return margins / 2;
	}
	Size RenderEngine::FromDIP(const Size& size) const
	{
		return m_Renderer.GetView()->FromDIP((wxSize)size);
	}

	size_t RenderEngine::FindFirstLineBreak(const String& string) const
	{
		for (size_t i = 0; i < string.size(); i++)
		{
			const wxChar c = string[i];
			if (c == wxS('\r') || c == wxS('\n'))
			{
				return i;
			}
		}
		return String::npos;
	}
	int RenderEngine::GetControlFlags(CellState cellState) const
	{
		const Column* column = m_Renderer.GetColumn();

		int flags = wxCONTROL_NONE;
		if (!m_Renderer.GetAttributes().Options().ContainsOption(CellOption::Enabled))
		{
			flags |= wxCONTROL_DISABLED;
		}
		if (cellState.IsSelected())
		{
			flags |= wxCONTROL_PRESSED|wxCONTROL_SELECTED;
		}
		if (column && (cellState.IsHotTracked() && column->IsHotTracked()))
		{
			flags |= wxCONTROL_CURRENT|wxCONTROL_FOCUSED;
		}

		return flags;
	}
	String RenderEngine::StripMarkup(const String& markup) const
	{
		// Stub for now. Need proper mnemonics removal algorithm
		return wxControl::RemoveMnemonics(markup);
	}

	Size RenderEngine::GetTextExtent(const String& string) const
	{
		if (GDIContext dc = GetTextRenderingDC())
		{
			return GetTextExtent(dc, string);
		}
		else
		{
			// No existing window context right now, create one to measure text
			GDIClientContext clientDC(*m_Renderer.GetView());
			return GetTextExtent(clientDC, string);
		}
	}
	Size RenderEngine::GetTextExtent(GDIContext& dc, const String& string) const
	{
		const CellAttribute& attributes = m_Renderer.GetAttributes();

		if (m_Renderer.IsMarkupEnabled())
		{
			GDIAction::ChangeFont fontChnager(dc);
			if (attributes.FontOptions().NeedDCAlteration())
			{
				fontChnager.Set(attributes.GetEffectiveFont(dc.GetFont()));
			}

			return Markup::GetTextExtent(m_Renderer.m_MarkupMode, dc, string);
		}
		else
		{
			auto GetEffectiveFontIfNeeded = [&dc, &attributes]() -> Font
			{
				if (attributes.FontOptions().NeedDCAlteration())
				{
					return attributes.GetEffectiveFont(dc.GetFont());
				}
				return {};
			};
			auto MeasureString = [&dc](const String& text, const Font& font = {})
			{
				return dc.GetTextExtent(text, font).GetExtent();
			};

			const size_t lineBreakPos = FindFirstLineBreak(string);
			if (lineBreakPos != String::npos)
			{
				return MeasureString(string.Left(lineBreakPos), GetEffectiveFontIfNeeded());
			}
			else
			{
				return MeasureString(string, GetEffectiveFontIfNeeded());
			}
		}
	}

	Size RenderEngine::GetMultilineTextExtent(const String& string) const
	{
		if (GDIContext dc = GetTextRenderingDC())
		{
			return GetMultilineTextExtent(dc, string);
		}
		else
		{
			// No existing window context right now, create one to measure text
			GDIClientContext clientDC(*m_Renderer.GetView());
			return GetMultilineTextExtent(clientDC, string);
		}
	}
	Size RenderEngine::GetMultilineTextExtent(GDIContext& dc, const String& string) const
	{
		// Markup doesn't support multiline text so we are ignoring it for now.

		const CellAttribute& attributes = m_Renderer.GetAttributes();

		GDIAction::ChangeFont fontChnager(dc);
		if (attributes.FontOptions().NeedDCAlteration())
		{
			fontChnager.Set(attributes.GetEffectiveFont(dc.GetFont()));
		}

		return dc.GetMultiLineTextExtent(string);
	}

	bool RenderEngine::DrawText(const Rect& cellRect, CellState cellState, const String& string, int offsetX)
	{
		if (GDIContext dc = GetTextRenderingDC())
		{
			return DrawText(dc, cellRect, cellState, string, offsetX);
		}
		return false;
	}
	bool RenderEngine::DrawText(GDIContext& dc, const Rect& cellRect, CellState cellState, const String& string, int offsetX)
	{
		if (!string.IsEmpty())
		{
			const CellAttribute& attributes = m_Renderer.GetAttributes();

			Rect textRect = cellRect;
			textRect.X() += offsetX;
			textRect.Width() -= offsetX;

			int flags = 0;
			if (m_Renderer.IsMarkupWithMnemonicsEnabled() && attributes.Options().ContainsOption(CellOption::ShowAccelerators))
			{
				flags |= wxMarkupText::Render_ShowAccels;
			}

			if (m_Renderer.IsMarkupEnabled())
			{
				Markup::DrawText(m_Renderer.m_MarkupMode, string, m_Renderer.GetView(), dc, textRect, flags, static_cast<wxEllipsizeMode>(m_Renderer.GetEllipsizeMode()));
			}
			else
			{
				auto DrawString = [this, &dc, &textRect](const String& text)
				{
					dc.DrawText(textRect.GetPosition(), wxControl::Ellipsize(text, dc.ToWxDC(), static_cast<wxEllipsizeMode>(m_Renderer.GetEllipsizeMode()), textRect.GetWidth()));
				};

				const size_t lineBreakPos = FindFirstLineBreak(string);
				if (lineBreakPos != String::npos)
				{
					DrawString(string.Left(lineBreakPos));
				}
				else
				{
					DrawString(string);
				}
			}
			return true;
		}
		return false;
	}

	bool RenderEngine::DrawBitmap(const Rect& cellRect, CellState cellState, const Bitmap& bitmap, int reservedWidth)
	{
		if (bitmap)
		{
			wxGraphicsContext& context = m_Renderer.GetGraphicsContext();
			const CellAttribute& attributes = m_Renderer.GetAttributes();

			auto DrawBitmap = [&]()
			{
				const Point pos = cellRect.GetPosition();
				const Size size = bitmap.GetSize();
				const bool isEnabled = attributes.Options().ContainsOption(CellOption::Enabled);

				context.DrawBitmap((isEnabled ? bitmap : bitmap.ConvertToDisabled()).ToWxBitmap(), pos.GetX(), pos.GetY(), size.GetWidth(), size.GetHeight());
			};

			if (bitmap.GetSize() > cellRect.GetSize())
			{
				GCClip clip(context, cellRect);
				DrawBitmap();
			}
			else
			{
				DrawBitmap();
			}
			return true;
		}
		else if (reservedWidth > 0)
		{
			return true;
		}
		return false;
	}
	int RenderEngine::DrawBitmapWithText(const Rect& cellRect, CellState cellState, int offsetX, const String& text, const Bitmap& bitmap, bool centerTextV, int reservedWidth)
	{
		if (bitmap || reservedWidth > 0)
		{
			DrawBitmap(Rect(cellRect.GetX() + offsetX, cellRect.GetY(), cellRect.GetWidth() - offsetX, cellRect.GetHeight()), cellState, bitmap, reservedWidth);
			offsetX += (reservedWidth > 0 ? reservedWidth : bitmap.GetSize().GetWidth()) + FromDIPX(GetInterTextSpacing());
		}
		if (!text.IsEmpty())
		{
			if (bitmap && centerTextV)
			{
				const Rect textRect = CenterTextInside(cellRect, GetTextExtent(text));
				DrawText(textRect, cellState, text, offsetX);
			}
			else
			{
				DrawText(cellRect, cellState, text, offsetX);
			}
		}
		return offsetX;
	}
	bool RenderEngine::DrawProgressBar(const Rect& cellRect, CellState cellState, int value, int range, ProgressState state, Color* averageBackgroundColor)
	{
		using namespace kxf;

		// Progress bar looks really ugly when it's smaller than 10x10 pixels,
		// so don't draw it at all in this case.
		const Size minSize = FromDIP(10, 10);
		if (cellRect.GetWidth() < minSize.GetWidth() || cellRect.GetHeight() < minSize.GetHeight())
		{
			return false;
		}

		if (UxTheme theme(*m_Renderer.GetView(), UxThemeClass::Progress); theme)
		{
			switch (state)
			{
				case ProgressState::Paused:
				{
					theme.DrawProgressBar(m_Renderer.GetRegularDC(), PP_BAR, PP_FILL, PBFS_PAUSED, cellRect, value, range, averageBackgroundColor);
					break;
				}
				case ProgressState::Error:
				{
					theme.DrawProgressBar(m_Renderer.GetRegularDC(), PP_BAR, PP_FILL, PBFS_ERROR, cellRect, value, range, averageBackgroundColor);
					break;
				}
				case ProgressState::Partial:
				{
					theme.DrawProgressBar(m_Renderer.GetRegularDC(), PP_BAR, PP_FILL, PBFS_PARTIAL, cellRect, value, range, averageBackgroundColor);
					break;
				}
				default:
				{
					theme.DrawProgressBar(m_Renderer.GetRegularDC(), PP_BAR, PP_FILL, PBFS_NORMAL, cellRect, value, range, averageBackgroundColor);
					break;
				}
			};
			return true;
		}

		wxRendererNative::Get().DrawGauge(m_Renderer.GetView(), m_Renderer.GetGraphicsDC().ToWxDC(), cellRect, value, range);
		return true;
	}

	Size RenderEngine::GetToggleSize() const
	{
		return wxRendererNative::Get().GetCheckBoxSize(m_Renderer.GetView());
	}
	Size RenderEngine::DrawToggle(GDIContext& dc, const Rect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType)
	{
		int flags = GetControlFlags(cellState);
		switch (toggleState)
		{
			case ToggleState::Checked:
			{
				flags |= wxCONTROL_CHECKED;
				break;
			}
			case ToggleState::Indeterminate:
			{
				flags |= wxCONTROL_UNDETERMINED;
				break;
			}
		};

		// Ensure that the check boxes always have at least the minimal required size,
		// otherwise DrawCheckBox() doesn't really work well. If this size is greater than
		// the cell size, the checkbox will be truncated but this is a lesser evil.
		View* view = m_Renderer.GetView();
		Rect toggleRect = cellRect;
		Size size = toggleRect.GetSize();
		size.IncTo(GetToggleSize());
		toggleRect.SetSize(size);

		if (toggleType == ToggleType::CheckBox || flags & wxCONTROL_UNDETERMINED)
		{
			wxRendererNative::Get().DrawCheckBox(view, dc.ToWxDC(), toggleRect, flags);
		}
		else
		{
			wxRendererNative::Get().DrawRadioBitmap(view, dc.ToWxDC(), toggleRect, flags);
		}
		return toggleRect.GetSize();
	}
}

namespace kxf::UI::DataView
{
	void RenderEngine::DrawPlusMinusExpander(wxWindow* window, GDIContext& dc, const Rect& canvasRect, int flags)
	{
		const bool isActive = flags & wxCONTROL_CURRENT;
		const bool isExpanded = flags & wxCONTROL_EXPANDED;

		Rect rect(canvasRect.GetPosition(), canvasRect.GetSize() / 2);
		if (rect.GetWidth() % 2 == 0)
		{
			rect.X()++;
			rect.Width()--;
		}
		if (rect.GetHeight() % 2 == 0)
		{
			rect.Y()++;
			rect.Height()--;
		}
		rect.X() += rect.GetWidth() / 2;
		rect.Y() += rect.GetHeight() / 2;

		// Draw inner rectangle
		dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		dc.DrawRectangle(rect);

		// Draw border
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(wxSystemSettings::GetColour(isActive ? wxSYS_COLOUR_HOTLIGHT : wxSYS_COLOUR_MENUHILIGHT));
		dc.DrawRectangle(rect);

		// Draw plus/minus
		dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT), window->FromDIP(1), wxPENSTYLE_SOLID));

		const int width = std::min(rect.GetWidth(), rect.GetHeight());
		const int length = width * 0.5;

		int baseX = width * 0.2 + 1;
		int baseY = width / 2;
		auto GetXY = [&]()
		{
			return Point(rect.GetX() + baseX, rect.GetY() + baseY);
		};

		// Draw horizontal line
		Point pos = GetXY();
		dc.DrawLine(pos, {pos.GetX() + length, pos.GetY()});
		if (isExpanded)
		{
			return;
		}

		// Draw vertical line
		std::swap(baseX, baseY);
		pos = GetXY();
		dc.DrawLine(pos, {pos.GetX(), pos.GetY() + length});
	}
	void RenderEngine::DrawSelectionRect(wxWindow* window, GDIContext& dc, const Rect& cellRect, int flags)
	{
		using namespace kxf;

		if (UxTheme theme(*window, UxThemeClass::TreeView); theme)
		{
			const int itemState = GetTreeItemState(flags);
			theme.DrawBackground(dc, TVP_TREEITEM, GetTreeItemState(flags), cellRect);
		}
		else
		{
			wxRendererNative::Get().DrawItemSelectionRect(window, dc.ToWxDC(), cellRect, flags);
		}
	}
}
