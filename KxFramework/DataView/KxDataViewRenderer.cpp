#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/KxImageList.h"
#include "KxFramework/KxGraphicsContext.h"
#include "KxFramework/KxDCClipper.h"

#include <wx/wxprec.h>
#include <wx/msw/private.h>
#include <wx/msw/uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>

wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewRenderer, wxObject);

namespace
{
	bool operator>(const wxSize& v1, const wxSize& v2)
	{
		return v1.GetWidth() > v2.GetWidth() || v1.GetHeight() > v2.GetHeight();
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
		KxColor color = m_Attributes.GetBackgroundColor();
		uint8_t alpha = color.GetA();
		if (alpha != 0 || alpha == 255)
		{
			color.SetA(70);

			wxDCPenChanger changePen(dc, color);
			wxDCBrushChanger changeBrush(dc, color);
			dc.DrawRectangle(cellRect);
		}
	}

	DrawCellBackground(cellRect, cellState);

	if (cellState & KxDATAVIEW_CELL_SELECTED && GetColumn()->IsCurrent())
	{
		wxRendererNative::Get().DrawFocusRect(GetView(), dc, wxRect(cellRect).Deflate(GetView()->FromDIP(wxSize(1, 1))));
	}
}
void KxDataViewRenderer::CallDrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
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

	if (cellSize.x >= 0 && cellSize.x < cellRect.width)
	{
		if (alignment & wxALIGN_CENTER_HORIZONTAL)
		{
			cellRectNew.x += DoCalcCenter(cellRect.width, cellSize.x);
		}
		else if (alignment & wxALIGN_RIGHT)
		{
			cellRectNew.x += cellRect.width - cellSize.x;
		}
		// else: wxALIGN_LEFT is the default

		cellRectNew.width = cellSize.x;
	}
	if (cellSize.y >= 0 && cellSize.y < cellRect.height)
	{
		if (alignment & wxALIGN_CENTER_VERTICAL)
		{
			cellRectNew.y += DoCalcCenter(cellRect.height, cellSize.y);
		}
		else if (alignment & wxALIGN_BOTTOM)
		{
			cellRectNew.y += cellRect.height - cellSize.y;
		}
		// else: wxALIGN_TOP is the default

		cellRectNew.height = cellSize.y;
	}

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
	return wxSize(KxDVC_DEFAULT_WIDTH, GetMainWindow()->GetUniformRowHeight());
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
			GetView()->GetTextExtent(string.Left(newLinePos), &size.x, &size.y, NULL, NULL, &font);
		}
		else
		{
			GetView()->GetTextExtent(string, &size.x, &size.y, NULL, NULL, &font);
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

		int newLinePos = DoFindFirstNewLinePos(string);
		if (newLinePos != wxNOT_FOUND)
		{
			wxRendererNative::Get().DrawItemText(GetView(), GetDC(), string.Left(newLinePos), textRect, GetEffectiveAlignment(), flags, GetEllipsizeMode());
		}
		else
		{
			wxRendererNative::Get().DrawItemText(GetView(), GetDC(), string, textRect, GetEffectiveAlignment(), flags, GetEllipsizeMode());
		}

		#if 0
		if (m_Attributes.HasCategoryLine())
		{
			wxPoint pos1(cellRect.GetRight() + 5, cellRect.GetHeight() / 2 + 1);
			wxPoint pos2(GetColumn()->GetWidth(), pos1.y);

			wxDCPenChanger pen(GetDC(), wxPen(m_Attributes.GetForegroundColor(), GetView()->FromDIP(1)));
			GetDC().DrawLine(pos1, pos2);
		}
		#endif
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
			bool hasMask = bitmap.GetMask() != NULL;
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
	wxUxThemeEngine* themeEngine = wxUxThemeEngine::GetIfActive();
	if (themeEngine)
	{
		wxUxThemeHandle themeHandle(GetView(), L"PROGRESS");
		if (themeHandle)
		{
			HDC dc = GetDC().GetHDC();
			
			// Draw background
			RECT cellRectWin = KxUtility::CopyRectToRECT(cellRect);
			themeEngine->DrawThemeBackground(themeHandle, dc, PP_BAR, 0, &cellRectWin, NULL);

			// Draw filled part
			RECT contentRect = {0};
			themeEngine->GetThemeBackgroundContentRect(themeHandle, dc, PP_BAR, 0, &cellRectWin, &contentRect);

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
					themeEngine->DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_PAUSED, &contentRect, NULL);
					break;
				}
				case ProgressBarState::Error:
				{
					themeEngine->DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_ERROR, &contentRect, NULL);
					break;
				}
				case ProgressBarState::Partial:
				{
					themeEngine->DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_PARTIAL, &contentRect, NULL);
					break;
				}

				default:
				case ProgressBarState::Normal:
				{
					themeEngine->DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_NORMAL, &contentRect, NULL);
					break;
				}
			};
			return true;
		}
	}

	wxRendererNative::Get().DrawGauge(GetView(), GetGCDC(), cellRect, value, range);
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
