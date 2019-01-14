#include "KxStdAfx.h"
#include "Renderer.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include "KxFramework/KxUxTheme.h"
#include "wx/generic/private/markuptext.h"

namespace
{
	bool operator>(const wxSize& v1, const wxSize& v2)
	{
		return v1.GetWidth() > v2.GetWidth() || v1.GetHeight() > v2.GetHeight();
	}
}

namespace Kx::DataView2::Markup
{
	using MarkupMode = Renderer::MarkupMode;
	template<MarkupMode t_Mode> auto Create(const wxString& string = {})
	{
		if constexpr(t_Mode == MarkupMode::TextOnly)
		{
			return wxItemMarkupText(string);
		}
		else if constexpr(t_Mode == MarkupMode::WithMnemonics)
		{
			return wxMarkupText(string);
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
				return GetTextExtent(Create<MarkupMode::TextOnly>(string), dc);
			}
			case MarkupMode::WithMnemonics:
			{
				return GetTextExtent(Create<MarkupMode::WithMnemonics>(string), dc);
			}
		};
		return wxSize(0, 0);
	}
	
	template<class T> void DrawText(T& markup, wxWindow* window, wxDC& dc, const wxRect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		if constexpr(std::is_same_v<T, wxMarkupText>)
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

namespace Kx::DataView2
{
	void Renderer::SetupCellAttributes(const Node& node, Column& column, CellState cellState)
	{
		m_Column = &column;

		// Now check if we have a value and remember it for rendering it later.
		// Notice that we do it even if it's null, as the cell should be empty then
		// and not show the last used value.
		SetValue(node.GetValue(column));

		// Set up the attributes for this item if it's not empty.
		// Reset attributes if they are not needed.
		m_Attributes.Reset();
		if (!node.GetAttributes(m_Attributes, cellState, column))
		{
			m_Attributes.Reset();
		}
	}
	void Renderer::CallDrawCellBackground(const wxRect& cellRect, CellState cellState)
	{
		wxDC& dc = GetGraphicsDC();

		if (m_Attributes.HasHeaderBackgound())
		{
			wxSize offsetSize = GetView()->FromDIP(wxSize(0, 1));

			wxRect buttonRect = cellRect;
			buttonRect.width += offsetSize.x;
			buttonRect.height += offsetSize.y;

			wxBitmap canvas(cellRect.GetSize(), 32);
			wxMemoryDC memDC(canvas);
			wxRendererNative::Get().DrawHeaderButton(GetView(), memDC, wxRect(-1, 0, buttonRect.GetWidth() + 1, buttonRect.GetHeight()), DoGetControlFlags(cellState), wxHDR_SORT_ICON_NONE);
			if (!cellState.IsSelected())
			{
				KxColor lineColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
				lineColor.SetA(48);

				memDC.SetPen(lineColor);
				memDC.DrawLine(wxPoint(0, 0), wxPoint(buttonRect.GetWidth() + 1, 0));
			}

			dc.DrawBitmap(canvas, cellRect.GetPosition());
		}

		if (m_Attributes.HasBackgroundColor())
		{
			const KxColor& color = m_Attributes.GetBackgroundColor();
			wxDCPenChanger changePen(dc, color);
			wxDCBrushChanger changeBrush(dc, color);
			dc.DrawRectangle(cellRect);
		}
		DrawCellBackground(cellRect, cellState);

		if (cellState.IsSelected() && GetColumn()->IsCurrent())
		{
			wxRendererNative::Get().DrawFocusRect(GetView(), dc, wxRect(cellRect).Deflate(GetView()->FromDIP(wxSize(1, 1))));
		}
	}
	void Renderer::CallDrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		wxGCDC& dc = GetGraphicsDC();

		// Change text color
		wxDCTextColourChanger chnageTextColor(dc);
		if (m_Attributes.HasForegroundColor())
		{
			KxColor color = m_Attributes.GetForegroundColor();
			if (!m_Attributes.IsEnabled())
			{
				color.MakeDisabled();
			}
			chnageTextColor.Set(color);
		}
		else if (!m_Attributes.IsEnabled())
		{
			chnageTextColor.Set(GetView()->GetForegroundColour().MakeDisabled());
		}

		// Change font
		wxDCFontChanger changeFont(GetGraphicsDC());
		if (m_Attributes.HasFontAttributes())
		{
			changeFont.Set(m_Attributes.GetEffectiveFont(dc.GetFont()));
		}

		// Adjust the rectangle ourselves to account for the alignment
		wxRect cellRectNew = cellRect;
		const wxAlignment alignment = m_Attributes.HasAlignment() ? m_Attributes.GetAlignment() : GetEffectiveAlignment();
		const wxSize cellSize = GetCellSize();

		// Take alignment into account only if there is enough space, otherwise
		// show as much contents as possible.
		// 
		// Notice that many existing renderers (e.g. wxDataViewSpinRenderer)
		// return hard-coded size which can be more than they need and if we
		// trusted their GetSize() we'd draw the text out of cell bounds entirely

		if (cellSize.GetWidth() >= 0 && cellSize.GetWidth() < cellRect.GetWidth())
		{
			if (alignment & wxALIGN_CENTER_HORIZONTAL)
			{
				cellRectNew.x += DoCalcCenter(cellRect.GetWidth(), cellSize.GetWidth());
			}
			else if (alignment & wxALIGN_RIGHT)
			{
				cellRectNew.x += cellRect.GetWidth() - cellSize.GetWidth();
			}
			// else: wxALIGN_LEFT is the default

			cellRectNew.SetWidth(cellSize.GetWidth());
		}
		if (cellSize.GetHeight() >= 0 && cellSize.GetHeight() < cellRect.GetHeight())
		{
			if (alignment & wxALIGN_CENTER_VERTICAL)
			{
				cellRectNew.y += DoCalcCenter(cellRect.GetHeight(), cellSize.GetHeight());
			}
			else if (alignment & wxALIGN_BOTTOM)
			{
				cellRectNew.y += cellRect.GetHeight() - cellSize.GetHeight();
			}
			// else: wxALIGN_TOP is the default

			cellRectNew.SetHeight(cellSize.y);
		}
		DrawCellContent(cellRectNew, cellState);
	}

	bool Renderer::HasSolidBackground() const
	{
		return !HasSpecialBackground();
	}
	bool Renderer::HasSpecialBackground() const
	{
		return m_Attributes.HasHeaderBackgound();
	}
	wxSize Renderer::GetCellSize() const
	{
		return wxSize(0, 0);
	}

	int Renderer::DoCalcCenter(int cellSize, int itemSize) const
	{
		const int margins = cellSize - itemSize;

		// Can't offset by fractional values, so return 1.
		if (margins > 0 && margins <= 3)
		{
			return 1;
		}
		return margins / 2;
	}
	int Renderer::DoFindFirstNewLinePos(const wxString& string) const
	{
		int pos = string.Find('\r');
		if (pos == wxNOT_FOUND)
		{
			pos = string.Find('\n');
		}
		return pos;
	}
	int Renderer::DoGetControlFlags(CellState cellState) const
	{
		int flags = wxCONTROL_NONE;
		if (cellState.IsSelected())
		{
			flags |= wxCONTROL_PRESSED;
		}
		if (cellState.IsHighlighted())
		{
			flags |= wxCONTROL_CURRENT|wxCONTROL_FOCUSED;
		}
		if (!m_Attributes.IsEnabled())
		{
			flags |= wxCONTROL_DISABLED;
		}
		return flags;
	}

	wxSize Renderer::DoGetTextExtent(const wxString& string) const
	{
		// Regular (GDI) device context is preferable to draw and measure text
		if (HasRegularDC())
		{
			return DoGetTextExtent(GetRegularDC(), string);
		}
		else if (HasGraphicsDC())
		{
			return DoGetTextExtent(GetGraphicsDC(), string);
		}
		else
		{
			// No existing window context right now, create one to measure text
			wxClientDC dc(GetView());
			return DoGetTextExtent(dc, string);
		}
	}
	wxSize Renderer::DoGetTextExtent(wxDC& dc, const wxString& string) const
	{
		wxSize textExtent(0, 0);
		if (m_Attributes.HasFontAttributes())
		{
			wxFont font(m_Attributes.GetEffectiveFont(dc.GetFont()));
			int newLinePos = DoFindFirstNewLinePos(string);

			if (newLinePos != wxNOT_FOUND)
			{
				if (IsMarkupEnabled())
				{
					textExtent = Markup::GetTextExtent(m_MarkupMode, dc, string);
				}
				else
				{
					dc.GetTextExtent(string.Left(newLinePos), &textExtent.x, &textExtent.y, nullptr, nullptr, &font);
				}
			}
			else
			{
				if (IsMarkupEnabled())
				{
					textExtent = Markup::GetTextExtent(m_MarkupMode, dc, string);
				}
				else
				{
					dc.GetTextExtent(string, &textExtent.x, &textExtent.y, nullptr, nullptr, &font);
				}
			}
			
		}
		else
		{
			textExtent = dc.GetTextExtent(string);
		}
		return textExtent;
	}

	bool Renderer::DoDrawText(const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX)
	{
		if (HasRegularDC())
		{
			return DoDrawText(GetRegularDC(), cellRect, cellState, string, offsetX);
		}
		else if (HasGraphicsDC())
		{
			return DoDrawText(GetGraphicsDC(), cellRect, cellState, string, offsetX);
		}
		return false;
	}
	bool Renderer::DoDrawText(wxDC& dc, const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX)
	{
		if (!string.IsEmpty())
		{
			wxRect textRect = cellRect;
			textRect.x += offsetX;
			textRect.width -= offsetX;

			int flags = 0;
			if (IsMarkupWithMnemonicsEnabled() && m_Attributes.ShouldShowAccelerators())
			{
				flags |= wxMarkupText::Render_ShowAccels;
			}

			int newLinePos = DoFindFirstNewLinePos(string);
			if (newLinePos != wxNOT_FOUND)
			{
				if (IsMarkupEnabled())
				{
					Markup::DrawText(m_MarkupMode, string, GetView(), dc, textRect, flags, GetEllipsizeMode());
				}
				else
				{
					dc.DrawText(wxControl::Ellipsize(string, dc, GetEllipsizeMode(), textRect.GetWidth()), textRect.GetPosition());
				}
			}
			else
			{
				if (IsMarkupEnabled())
				{
					Markup::DrawText(m_MarkupMode, string, GetView(), dc, textRect, flags, GetEllipsizeMode());
				}
				else
				{
					dc.DrawText(wxControl::Ellipsize(string, dc, GetEllipsizeMode(), textRect.GetWidth()), textRect.GetPosition());
				}
			}
			return true;
		}
		return false;
	}

	bool Renderer::DoDrawBitmap(const wxRect& cellRect, CellState cellState, const wxBitmap& bitmap)
	{
		if (bitmap.IsOk())
		{
			const wxPoint pos = cellRect.GetPosition();
			const wxSize size = bitmap.GetSize();

			GetGraphicsContext().DrawBitmap(m_Attributes.IsEnabled() ? bitmap : bitmap.ConvertToDisabled(), pos.x, pos.y, size.GetWidth(), size.GetHeight());
			return true;
		}
		return false;
	}
	bool Renderer::DoDrawProgressBar(const wxRect& cellRect, CellState cellState, int value, int range, ProgressBarState state)
	{
		KxUxTheme::Handle themeHandle(GetView(), L"PROGRESS");
		if (themeHandle)
		{
			HDC dc = GetGraphicsDC().GetHDC();

			// Draw background
			RECT cellRectWin = KxUtility::CopyRectToRECT(cellRect);
			::DrawThemeBackground(themeHandle, dc, PP_BAR, 0, &cellRectWin, nullptr);

			// Draw filled part
			RECT contentRect = {0};
			::GetThemeBackgroundContentRect(themeHandle, dc, PP_BAR, 0, &cellRectWin, &contentRect);

			contentRect.right = contentRect.left + wxMulDivInt32(contentRect.right - contentRect.left, value, range);
			if (contentRect.left - 2 == cellRectWin.left)
			{
				contentRect.left++;
			}
			if (contentRect.right + 2 == cellRectWin.right)
			{
				contentRect.right--;
			}

			switch (state)
			{
				case ProgressBarState::Paused:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_PAUSED, &contentRect, nullptr);
					break;
				}
				case ProgressBarState::Error:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_ERROR, &contentRect, nullptr);
					break;
				}
				case ProgressBarState::Partial:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_PARTIAL, &contentRect, nullptr);
					break;
				}

				default:
				case ProgressBarState::Normal:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_NORMAL, &contentRect, nullptr);
					break;
				}
			};
			return true;
		}

		wxRendererNative::Get().DrawGauge(GetView(), GetGraphicsDC(), cellRect, value, range);
		return true;
	}

	MainWindow* Renderer::GetMainWindow() const
	{
		return m_Column ? m_Column->GetView()->GetMainWindow() : nullptr;
	}
	View* Renderer::GetView() const
	{
		return m_Column ? m_Column->GetView() : nullptr;
	}

	wxAlignment Renderer::GetEffectiveAlignment() const
	{
		if (m_Alignment == wxALIGN_INVALID)
		{
			// If we don't have an explicit alignment ourselves, use that of the
			// column in horizontal direction and default vertical alignment

			wxAlignment columnAlignment = m_Column->GetTitleAlignment();
			if (columnAlignment == wxALIGN_INVALID)
			{
				columnAlignment = wxALIGN_LEFT;
			}
			return static_cast<wxAlignment>(columnAlignment|wxALIGN_CENTER_VERTICAL);
		}
		return m_Alignment;
	}
}
