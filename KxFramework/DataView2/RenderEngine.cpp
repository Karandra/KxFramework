#include "KxStdAfx.h"
#include "RenderEngine.h"
#include "Renderer.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "KxFramework/KxUxTheme.h"
#include "Kx/Drawing/GCOperations.h"
#include "wx/generic/private/markuptext.h"

#include "KxFramework/KxUxThemePartsAndStates.h"

namespace
{
	bool operator<(const wxSize& left, const wxSize& right)
	{
		return left.GetWidth() < right.GetWidth() || left.GetHeight() < right.GetHeight();
	}
	bool operator>(const wxSize& left, const wxSize& right)
	{
		return left.GetWidth() > right.GetWidth() || left.GetHeight() > right.GetHeight();
	}

	int GetTreeItemState(int flags)
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

namespace KxDataView2::Markup
{
	class TextOnly: public wxItemMarkupText
	{
		public:
			TextOnly(const wxString& markup)
				:wxItemMarkupText(markup)
			{
			}

		public:
			wxSize GetTextExtent(wxDC& dc) const
			{
				return Measure(dc);
			}
	};
	class WithMnemonics: public wxMarkupText
	{
		public:
			WithMnemonics(const wxString& markup)
				:wxMarkupText(markup)
			{
			}

		public:
			wxSize GetTextExtent(wxDC& dc) const
			{
				return Measure(dc);
			}
	};
}

namespace KxDataView2::Markup
{
	using MarkupMode = Renderer::MarkupMode;
	template<MarkupMode t_Mode> auto Create(const wxString& string = {})
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

	wxSize GetTextExtent(const wxMarkupTextBase& markup, wxDC& dc)
	{
		return markup.Measure(dc);
	}
	wxSize GetTextExtent(MarkupMode mode, wxDC& dc, const wxString& string)
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
		return wxSize(0, 0);
	}
	
	template<class T> void DrawText(T& markup, wxWindow* window, wxDC& dc, const wxRect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		if constexpr(std::is_same_v<T, WithMnemonics>)
		{
			markup.Render(dc, rect, flags);
		}
		else
		{
			markup.Render(window, dc, rect, flags, ellipsizeMode);
		}
	}
	void DrawText(MarkupMode mode, const wxString& string, wxWindow* window, wxDC& dc, const wxRect& rect, int flags, wxEllipsizeMode ellipsizeMode)
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

namespace KxDataView2
{
	wxDC* RenderEngine::GetTextRenderingDC() const
	{
		if (m_Renderer.HasRegularDC() && !m_AlwaysUseGC)
		{
			return &m_Renderer.GetRegularDC();
		}
		else if (m_Renderer.HasGraphicsDC())
		{
			return &m_Renderer.GetGraphicsDC();
		}
		return nullptr;
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
	wxSize RenderEngine::FromDIP(const wxSize& size) const
	{
		return m_Renderer.GetView()->FromDIP(size);
	}

	size_t RenderEngine::FindFirstLineBreak(const wxString& string) const
	{
		for (size_t i = 0; i < string.size(); i++)
		{
			const wxChar c = string[i];
			if (c == wxS('\r') || c == wxS('\n'))
			{
				return i;
			}
		}
		return wxString::npos;
	}
	int RenderEngine::GetControlFlags(CellState cellState) const
	{
		const Column* column = m_Renderer.GetColumn();

		int flags = wxCONTROL_NONE;
		if (!m_Renderer.GetAttributes().Options().IsEnabled(CellOption::Enabled))
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
	wxString RenderEngine::StripMarkup(const wxString& markup) const
	{
		// Stub for now. Need proper mnemonics removal algorithm
		return wxControl::RemoveMnemonics(markup);
	}

	wxSize RenderEngine::GetTextExtent(const wxString& string) const
	{
		if (wxDC* dc = GetTextRenderingDC())
		{
			return GetTextExtent(*dc, string);
		}
		else
		{
			// No existing window context right now, create one to measure text
			wxClientDC clientDC(m_Renderer.GetView());
			return GetTextExtent(clientDC, string);
		}
	}
	wxSize RenderEngine::GetTextExtent(wxDC& dc, const wxString& string) const
	{
		const CellAttributes& attributes = m_Renderer.GetAttributes();

		if (m_Renderer.IsMarkupEnabled())
		{
			wxDCFontChanger fontChnager(dc);
			if (attributes.FontOptions().NeedDCAlteration())
			{
				fontChnager.Set(attributes.GetEffectiveFont(dc.GetFont()));
			}

			return Markup::GetTextExtent(m_Renderer.m_MarkupMode, dc, string);
		}
		else
		{
			auto GetEffectiveFontIfNeeded = [&dc, &attributes]()
			{
				if (attributes.FontOptions().NeedDCAlteration())
				{
					return attributes.GetEffectiveFont(dc.GetFont());
				}
				return wxNullFont;
			};
			auto MeasureString = [&dc](const wxString& text, const wxFont& font = wxNullFont)
			{
				wxSize extent;
				dc.GetTextExtent(text, &extent.x, &extent.y, nullptr, nullptr, font.IsOk() ? &font : nullptr);
				return extent;
			};

			const size_t lineBreakPos = FindFirstLineBreak(string);
			if (lineBreakPos != wxString::npos)
			{
				return MeasureString(string.Left(lineBreakPos), GetEffectiveFontIfNeeded());
			}
			else
			{
				return MeasureString(string, GetEffectiveFontIfNeeded());
			}
		}
	}

	wxSize RenderEngine::GetMultilineTextExtent(const wxString& string) const
	{
		if (wxDC* dc = GetTextRenderingDC())
		{
			return GetMultilineTextExtent(*dc, string);
		}
		else
		{
			// No existing window context right now, create one to measure text
			wxClientDC clientDC(m_Renderer.GetView());
			return GetMultilineTextExtent(clientDC, string);
		}
	}
	wxSize RenderEngine::GetMultilineTextExtent(wxDC& dc, const wxString& string) const
	{
		// Markup doesn't support multiline text so we are ignoring it for now.

		const CellAttributes& attributes = m_Renderer.GetAttributes();
		
		wxDCFontChanger fontChnager(dc);
		if (attributes.FontOptions().NeedDCAlteration())
		{
			fontChnager.Set(attributes.GetEffectiveFont(dc.GetFont()));
		}

		return dc.GetMultiLineTextExtent(string);
	}

	bool RenderEngine::DrawText(const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX)
	{
		if (wxDC* dc = GetTextRenderingDC())
		{
			return DrawText(*dc, cellRect, cellState, string, offsetX);
		}
		return false;
	}
	bool RenderEngine::DrawText(wxDC& dc, const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX)
	{
		if (!string.IsEmpty())
		{
			const CellAttributes& attributes = m_Renderer.GetAttributes();

			wxRect textRect = cellRect;
			textRect.x += offsetX;
			textRect.width -= offsetX;

			int flags = 0;
			if (m_Renderer.IsMarkupWithMnemonicsEnabled() && attributes.Options().IsEnabled(CellOption::ShowAccelerators))
			{
				flags |= wxMarkupText::Render_ShowAccels;
			}

			if (m_Renderer.IsMarkupEnabled())
			{
				Markup::DrawText(m_Renderer.m_MarkupMode, string, m_Renderer.GetView(), dc, textRect, flags, m_Renderer.GetEllipsizeMode());
			}
			else
			{
				auto DrawString = [this, &dc, &textRect](const wxString& text)
				{
					dc.DrawText(wxControl::Ellipsize(text, dc, m_Renderer.GetEllipsizeMode(), textRect.GetWidth()), textRect.GetPosition());
				};

				const size_t lineBreakPos = FindFirstLineBreak(string);
				if (lineBreakPos != wxString::npos)
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

	bool RenderEngine::DrawBitmap(const wxRect& cellRect, CellState cellState, const wxBitmap& bitmap, int reservedWidth)
	{
		if (bitmap.IsOk())
		{
			wxGraphicsContext& context = m_Renderer.GetGraphicsContext();
			const CellAttributes& attributes = m_Renderer.GetAttributes();

			auto DrawBitmap = [&]()
			{
				const wxPoint pos = cellRect.GetPosition();
				const wxSize size = bitmap.GetSize();
				const bool isEnabled = attributes.Options().IsEnabled(CellOption::Enabled);

				context.DrawBitmap(isEnabled ? bitmap : bitmap.ConvertToDisabled(), pos.x, pos.y, size.GetWidth(), size.GetHeight());
			};

			if (bitmap.GetSize() > cellRect.GetSize())
			{
				KxFramework::GCClip clip(context, cellRect);
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
	int RenderEngine::DrawBitmapWithText(const wxRect& cellRect, CellState cellState, int offsetX, const wxString& text, const wxBitmap& bitmap, bool centerTextV, int reservedWidth)
	{
		if (bitmap.IsOk() || reservedWidth > 0)
		{
			DrawBitmap(wxRect(cellRect.GetX() + offsetX, cellRect.GetY(), cellRect.GetWidth() - offsetX, cellRect.GetHeight()), cellState, bitmap, reservedWidth);
			offsetX += (reservedWidth > 0 ? reservedWidth : bitmap.GetWidth()) + FromDIPX(GetInterTextSpacing());
		}
		if (!text.IsEmpty())
		{
			if (bitmap.IsOk() && centerTextV)
			{
				const wxRect textRect = CenterTextInside(cellRect, GetTextExtent(text));
				DrawText(textRect, cellState, text, offsetX);
			}
			else
			{
				DrawText(cellRect, cellState, text, offsetX);
			}
		}
		return offsetX;
	}
	bool RenderEngine::DrawProgressBar(const wxRect& cellRect, CellState cellState, int value, int range, ProgressState state, KxColor* averageBackgroundColor)
	{
		// Progress bar looks really ugly when it's smaller than 10x10 pixels,
		// so don't draw it at all in this case.
		const wxSize minSize = FromDIP(10, 10);
		if (cellRect.GetWidth() < minSize.GetWidth() || cellRect.GetHeight() < minSize.GetHeight())
		{
			return false;
		}

		if (KxUxTheme theme(*m_Renderer.GetView(), KxUxThemeClass::Progress); theme)
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

		wxRendererNative::Get().DrawGauge(m_Renderer.GetView(), m_Renderer.GetGraphicsDC(), cellRect, value, range);
		return true;
	}

	wxSize RenderEngine::GetToggleSize() const
	{
		return wxRendererNative::Get().GetCheckBoxSize(m_Renderer.GetView());
	}
	wxSize RenderEngine::DrawToggle(wxDC& dc, const wxRect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType)
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
		wxRect toggleRect = cellRect;
		wxSize size = toggleRect.GetSize();
		size.IncTo(GetToggleSize());
		toggleRect.SetSize(size);

		if (toggleType == ToggleType::CheckBox || flags & wxCONTROL_UNDETERMINED)
		{
			wxRendererNative::Get().DrawCheckBox(view, dc, toggleRect, flags);
		}
		else
		{
			wxRendererNative::Get().DrawRadioBitmap(view, dc, toggleRect, flags);
		}
		return toggleRect.GetSize();
	}
}

namespace KxDataView2
{
	void RenderEngine::DrawPlusMinusExpander(wxWindow* window, wxDC& dc, const wxRect& canvasRect, int flags)
	{
		const bool isActive = flags & wxCONTROL_CURRENT;
		const bool isExpanded = flags & wxCONTROL_EXPANDED;

		wxRect rect(canvasRect.GetPosition(), canvasRect.GetSize() / 2);
		if (rect.width % 2 == 0)
		{
			rect.x++;
			rect.width--;
		}
		if (rect.height % 2 == 0)
		{
			rect.y++;
			rect.height--;
		}
		rect.x += rect.width / 2;
		rect.y += rect.height / 2;

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
			return wxPoint(rect.x + baseX, rect.y + baseY);
		};

		// Draw horizontal line
		wxPoint pos = GetXY();
		dc.DrawLine(pos, {pos.x + length, pos.y});
		if (isExpanded)
		{
			return;
		}

		// Draw vertical line
		std::swap(baseX, baseY);
		pos = GetXY();
		dc.DrawLine(pos, {pos.x, pos.y + length});
	}
	void RenderEngine::DrawSelectionRect(wxWindow* window, wxDC& dc, const wxRect& cellRect, int flags)
	{
		if (KxUxTheme theme(*window, KxUxThemeClass::TreeView); theme)
		{
			const int itemState = GetTreeItemState(flags);
			theme.DrawBackground(dc, TVP_TREEITEM, GetTreeItemState(flags), cellRect);
		}
		else
		{
			wxRendererNative::Get().DrawItemSelectionRect(window, dc, cellRect, flags);
		}
	}
}
