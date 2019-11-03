#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/KxImageList.h"
#include "KxFramework/KxGraphicsContext.h"
#include "KxFramework/KxDCClipper.h"
#include "KxFramework/KxUxTheme.h"
#include "KxFramework/KxUxThemePartsAndStates.h"
#include "wx/generic/private/markuptext.h"
#include "wx/generic/private/markuptext.h"

wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewRenderer, wxObject);

namespace
{
	bool operator>(const wxSize& v1, const wxSize& v2)
	{
		return v1.GetWidth() > v2.GetWidth() || v1.GetHeight() > v2.GetHeight();
	}

	using MarkupMode = KxDataViewRenderer::MarkupMode;
	template<MarkupMode t_Mode> auto CreateMarkup(const wxString& string = {})
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

	wxSize GetMarkupTextExtent(const wxMarkupTextBase& markup, wxWindow* window, const wxFont& font)
	{
		wxClientDC dc(window);
		dc.SetFont(font);
		return markup.Measure(dc);
	}
	wxSize GetMarkupTextExtent(MarkupMode mode, wxWindow* window, const wxFont& font, const wxString& string)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				return GetMarkupTextExtent(CreateMarkup<MarkupMode::TextOnly>(string), window, font);
			}
			case MarkupMode::WithMnemonics:
			{
				return GetMarkupTextExtent(CreateMarkup<MarkupMode::WithMnemonics>(string), window, font);
			}
		};
		return wxSize(0, 0);
	}
	
	template<class T> void DrawMarkupText(T& markup, wxWindow* window, wxDC& dc, const wxRect& rect, int flags, wxEllipsizeMode ellipsizeMode)
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
	void DrawMarkupText(MarkupMode mode, const wxString& string, wxWindow* window, wxDC& dc, const wxRect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				auto markup = CreateMarkup<MarkupMode::TextOnly>(string);
				return DrawMarkupText(markup, window, dc, rect, flags, ellipsizeMode);
			}
			case MarkupMode::WithMnemonics:
			{
				auto markup = CreateMarkup<MarkupMode::WithMnemonics>(string);
				return DrawMarkupText(markup, window, dc, rect, flags, ellipsizeMode);
			}
		};
	}
}

/* Drawing */
void KxDataViewRenderer::PrepareItemToDraw(const KxDataViewItem& item, KxDataViewCellState cellState)
{
	// Now check if we have a value and remember it for rendering it later.
	// Notice that we do it even if it's null, as the cell should be empty then
	// and not show the last used value.
	wxAny value;
	CheckedGetValue(value, item);
	SetValue(value);

	// Set up the attributes for this item if it's not empty.
	// Reset attributes if they are not needed.
	m_Attributes.Reset();
	if (!GetModel()->GetItemAttributes(item, m_Column, m_Attributes, cellState))
	{
		m_Attributes.Reset();
	}

	// Finally determine the enabled/disabled state and apply it.
	SetEnabled(GetModel()->IsEnabled(item, m_Column));
}
bool KxDataViewRenderer::CheckedGetValue(wxAny& value, const KxDataViewItem& item)
{
	GetModel()->GetValue(value, item, GetColumn());

	// We always allow the cell to be null
	if (!value.IsNull() && !ValidateValue(value))
	{
		wxLogDebug("Wrong type returned from the model for editor at column %d (%s)", GetColumn()->GetID(), GetColumn()->GetTitle());

		// Don't return data of mismatching type, this could be unexpected.
		value.MakeNull();
		return false;
	}
	return true;
}

void KxDataViewRenderer::CallDrawCellBackground(const wxRect& cellRect, KxDataViewCellState cellState)
{
	wxDC& dc = HasGCDC() ? GetGCDC() : GetDC();

	if (m_Attributes.HasButtonBackgound())
	{
		wxRect buttonRect = cellRect;
		wxRendererNative::Get().DrawPushButton(GetView(), dc, buttonRect.Inflate(GetView()->FromDIP(wxSize(1, 1))), DoGetControlFlags(cellState));
	}
	else if (m_Attributes.HasHeaderButtonBackgound())
	{
		wxSize offsetSize = GetView()->FromDIP(wxSize(0, 1));

		wxRect buttonRect = cellRect;
		buttonRect.width += offsetSize.x;
		buttonRect.height += offsetSize.y;

		wxBitmap canvas(cellRect.GetSize(), 32);
		wxMemoryDC memDC(canvas);
		wxRendererNative::Get().DrawHeaderButton(GetView(), memDC, wxRect(-1, 0, buttonRect.GetWidth() + 1, buttonRect.GetHeight()), DoGetControlFlags(cellState), wxHDR_SORT_ICON_NONE);
		if (!(cellState & KxDATAVIEW_CELL_SELECTED))
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

	#if 0
	if (m_Attributes.HasCategoryLine())
	{
		KxColor color = KxUtility::GetThemeColor_Caption(GetView());
		if (!color.IsOk())
		{
			color = m_Attributes.GetForegroundColor();
		}
		color.SetA(70);

		int offset = 0;
		int width = 0;
		int y = cellRect.GetY() + cellRect.GetHeight() / 2;

		if (GetColumn()->IsFirst())
		{
			offset = 5;
			width = GetCellSize().GetWidth() + offset;
		}
		if (GetColumn()->IsLast())
		{
			offset = 5;
		}
		dc.DrawLine(wxPoint(cellRect.GetLeft() + width, y), wxPoint(cellRect.GetRight() - width - offset, y));
	}
	#endif

	DrawCellBackground(cellRect, cellState);

	if (cellState & KxDATAVIEW_CELL_SELECTED && GetColumn()->IsCurrent())
	{
		wxRendererNative::Get().DrawFocusRect(GetView(), dc, wxRect(cellRect).Deflate(GetView()->FromDIP(wxSize(1, 1))));
	}
}
void KxDataViewRenderer::CallDrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	//wxDCTextColourChanger chnageTextColorGC(GetGCDC(), m_Attributes.HasForegroundColor() ? m_Attributes.GetForegroundColor() : KxColor(GetView()->GetForegroundColour()));
	wxDCTextColourChanger chnageTextColorDC(GetDC(), m_Attributes.HasForegroundColor() ? m_Attributes.GetForegroundColor() : KxColor(GetView()->GetForegroundColour()));

	//wxDCFontChanger changeFontGC(GetGCDC());
	wxDCFontChanger changeFontDC(GetDC());
	if (m_Attributes.HasFontAttributes())
	{
		wxFont font = m_Attributes.GetEffectiveFont(GetDC().GetFont());

		//changeFontGC.Set(font);
		changeFontDC.Set(font);
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

bool KxDataViewRenderer::IsEnabled() const
{
	return m_Enabled && m_Attributes.IsEnabled() && GetView()->IsThisEnabled();
}
void KxDataViewRenderer::SetEnabled(bool enabled)
{
	m_Enabled = enabled;
}

bool KxDataViewRenderer::HasSolidBackground() const
{
	return !HasSpecialBackground();
}
bool KxDataViewRenderer::HasSpecialBackground() const
{
	return m_Attributes.HasHeaderButtonBackgound() || m_Attributes.HasButtonBackgound();
}
wxSize KxDataViewRenderer::GetCellSize() const
{
	return wxSize(KxDVC_DEFAULT_WIDTH / 2, GetMainWindow()->GetUniformRowHeight());
}

int KxDataViewRenderer::DoCalcCenter(int cellSize, int itemSize) const
{
	const int margins = cellSize - itemSize;

	// Can't offset by fractional values, so return 1.
	if (margins > 0 && margins <= 3)
	{
		return 1;
	}
	return margins / 2;
}
int KxDataViewRenderer::DoFindFirstNewLinePos(const wxString& string) const
{
	int pos = string.Find('\r');
	if (pos == wxNOT_FOUND)
	{
		pos = string.Find('\n');
	}
	return pos;
}
int KxDataViewRenderer::DoGetControlFlags(KxDataViewCellState cellState) const
{
	int flags = wxCONTROL_NONE;
	if (cellState & KxDATAVIEW_CELL_SELECTED)
	{
		flags |= wxCONTROL_PRESSED;
	}
	if (cellState & KxDATAVIEW_CELL_HIGHLIGHTED)
	{
		flags |= wxCONTROL_CURRENT|wxCONTROL_FOCUSED;
	}
	if (!m_Attributes.IsEnabled())
	{
		flags |= wxCONTROL_DISABLED;
	}
	return flags;
}

wxSize KxDataViewRenderer::DoGetTextExtent(const wxString& string) const
{
	if (m_Attributes.HasFontAttributes())
	{
		wxFont font(m_Attributes.GetEffectiveFont(GetView()->GetFont()));
		int newLinePos = DoFindFirstNewLinePos(string);
		
		wxSize size;
		if (newLinePos != wxNOT_FOUND)
		{
			if (IsMarkupEnabled())
			{
				return GetMarkupTextExtent(m_MarkupMode, GetView(), font, string);
			}
			else
			{
				GetView()->GetTextExtent(string.Left(newLinePos), &size.x, &size.y, nullptr, nullptr, &font);
			}
		}
		else
		{
			if (IsMarkupEnabled())
			{
				return GetMarkupTextExtent(m_MarkupMode, GetView(), font, string);
			}
			else
			{
				GetView()->GetTextExtent(string, &size.x, &size.y, nullptr, nullptr, &font);
			}
		}
		return size;
	}
	else
	{
		return GetView()->GetTextExtent(string);
	}
}
bool KxDataViewRenderer::DoDrawText(const wxRect& cellRect, KxDataViewCellState cellState, const wxString& string, int offsetX)
{
	if (!string.IsEmpty())
	{
		wxRect textRect = cellRect;
		textRect.x += offsetX;
		textRect.width -= offsetX;

		int flags = 0;
		if (cellState & KxDATAVIEW_CELL_SELECTED)
		{
			// With 'wxCONTROL_SELECTED' this draws text in black regardless of DC foreground color.
			flags |= wxCONTROL_FOCUSED|wxCONTROL_CURRENT;
		}
		if (!m_Attributes.IsEnabled())
		{
			flags |= wxCONTROL_DISABLED;
		}
		if (IsMarkupWithMnemonicsEnabled() && m_Attributes.ShouldShowAccelerators())
		{
			flags |= wxMarkupText::Render_ShowAccels;
		}

		int newLinePos = DoFindFirstNewLinePos(string);
		if (newLinePos != wxNOT_FOUND)
		{
			if (IsMarkupEnabled())
			{
				DrawMarkupText(m_MarkupMode, string, GetView(), GetDC(), textRect, flags, GetEllipsizeMode());
			}
			else
			{
				wxRendererNative::Get().DrawItemText(GetView(), GetDC(), string.Left(newLinePos), textRect, GetEffectiveAlignment(), flags, GetEllipsizeMode());
			}
		}
		else
		{
			if (IsMarkupEnabled())
			{
				DrawMarkupText(m_MarkupMode, string, GetView(), GetDC(), textRect, flags, GetEllipsizeMode());
			}
			else
			{
				wxRendererNative::Get().DrawItemText(GetView(), GetDC(), string, textRect, GetEffectiveAlignment(), flags, GetEllipsizeMode());
			}
		}
		return true;
	}
	return false;
}
bool KxDataViewRenderer::DoDrawBitmap(const wxRect& cellRect, KxDataViewCellState cellState, const wxBitmap& bitmap)
{
	if (bitmap.IsOk())
	{
		// Rescale if needed
		wxSize bitmapSize = bitmap.GetSize();
		bool needsScaling = false;
		if (bitmap.GetHeight() > cellRect.GetHeight() && IsOptionEnabled(KxDVR_ALLOW_BITMAP_SCALEDOWN))
		{
			bitmapSize.x = ((double)cellRect.GetHeight() / (double)bitmap.GetHeight()) * (double)bitmap.GetWidth();
			bitmapSize.y = cellRect.GetHeight() - GetView()->FromDIP(2);
			needsScaling = true;
		}

		// Draw bitmap
		const wxPoint pos = cellRect.GetPosition();
		if (IsOptionEnabled(KxDVR_IMAGELIST_BITMAP_DRAWING))
		{
			bool hasMask = bitmap.GetMask() != nullptr;
			KxImageList list(bitmapSize.GetWidth(), bitmapSize.GetHeight(), hasMask, 1);

			if (needsScaling)
			{
				wxBitmap bitmapScaled = bitmap.ConvertToImage().Rescale(bitmap.GetWidth(), bitmapSize.GetHeight());
				list.Add(m_Attributes.IsEnabled() ? bitmapScaled : bitmapScaled.ConvertToDisabled());
			}
			else
			{
				list.Add(m_Attributes.IsEnabled() ? bitmap : bitmap.ConvertToDisabled());
			}

			int drawFlags = wxIMAGELIST_DRAW_NORMAL|wxIMAGELIST_DRAW_TRANSPARENT;
			if (cellState & KxDATAVIEW_CELL_SELECTED)
			{
				drawFlags |= wxIMAGELIST_DRAW_SELECTED;
			}
			if (cellState & KxDATAVIEW_CELL_HIGHLIGHTED)
			{
				drawFlags |= wxIMAGELIST_DRAW_FOCUSED;
			}

			return list.Draw(0, GetDC(), pos.x, pos.y, drawFlags, HasSolidBackground());
		}
		else
		{
			if (HasGCDC())
			{
				GetGraphicsContext()->DrawBitmap(m_Attributes.IsEnabled() ? bitmap : bitmap.ConvertToDisabled(), pos.x, pos.y, bitmapSize.GetWidth(), bitmapSize.GetHeight());
			}
			else
			{
				wxBitmap bitmapScaled = bitmap.ConvertToImage().Rescale(bitmap.GetWidth(), bitmapSize.GetHeight());
				GetDC().DrawBitmap(bitmapScaled, pos, true);
			}
			return true;
		}
	}
	return false;
}
bool KxDataViewRenderer::DoDrawProgressBar(const wxRect& cellRect, KxDataViewCellState cellState, int value, int range, ProgressBarState state)
{
	// Progress bar looks really ugly when it's smaller than 10x10 pixels,
	// so don't draw it at all in this case.
	if (cellRect.GetWidth() < 10 || cellRect.GetHeight() < 10)
	{
		return false;
	}

	if (KxUxTheme theme(*GetView(), KxUxThemeClass::Progress); theme)
	{
		switch (state)
		{
			case ProgressBarState::Paused:
			{
				theme.DrawProgress(GetDC(), PP_BAR, PP_FILL, PBFS_PAUSED, cellRect, value, range);
				break;
			}
			case ProgressBarState::Error:
			{
				theme.DrawProgress(GetDC(), PP_BAR, PP_FILL, PBFS_ERROR, cellRect, value, range);
				break;
			}
			case ProgressBarState::Partial:
			{
				theme.DrawProgress(GetDC(), PP_BAR, PP_FILL, PBFS_PARTIAL, cellRect, value, range);
				break;
			}
			default:
			{
				theme.DrawProgress(GetDC(), PP_BAR, PP_FILL, PBFS_NORMAL, cellRect, value, range);
				break;
			}
		};
		return true;
	}

	wxRendererNative::Get().DrawGauge(GetView(), GetDC(), cellRect, value, range);
	return true;
}

KxDataViewRenderer::KxDataViewRenderer(KxDataViewCellMode cellMode, int alignment)
	:m_CellMode(cellMode), m_Alignment(static_cast<wxAlignment>(alignment))
{
	//SetOptionFlags(KxDVR_ALLOW_BITMAP_SCALEDOWN);
}
KxDataViewRenderer::~KxDataViewRenderer()
{
}

wxAlignment KxDataViewRenderer::GetEffectiveAlignment() const
{
	if (m_Alignment == wxALIGN_INVALID)
	{
		// If we don't have an explicit alignment ourselves, use that of the
		// column in horizontal direction and default vertical alignment

		wxAlignment columnAlignment = m_Column->GetAlignment();
		if (columnAlignment == wxALIGN_INVALID)
		{
			columnAlignment = wxALIGN_LEFT;
		}
		return static_cast<wxAlignment>(columnAlignment|wxALIGN_CENTER_VERTICAL);
	}
	return m_Alignment;
}
