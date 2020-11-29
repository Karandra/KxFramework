#include "stdafx.h"
#include "GDIRendererNative.h"
#include "GDIAction.h"
#include "../UxTheme.h"
#include "../Private/UxThemeDefines.h"

namespace
{
	using namespace kxf;

	constexpr int GetListItemState(FlagSet<NativeWidgetFlag> flags) noexcept
	{
		if (flags.Contains(NativeWidgetFlag::Disabled))
		{
			return LISS_DISABLED;
		}

		int itemState = flags.Contains(NativeWidgetFlag::Current) ? LISS_HOT : LISS_NORMAL;
		if (flags.Contains(NativeWidgetFlag::Selected))
		{
			itemState = flags.Contains(NativeWidgetFlag::Current) ? LISS_HOTSELECTED : LISS_SELECTED;
			if (!flags.Contains(NativeWidgetFlag::Focused))
			{
				itemState = LISS_SELECTEDNOTFOCUS;
			}
		}
		return itemState;
	};
	constexpr wxHeaderSortIconType MapSortArrow(Direction sortArrow) noexcept
	{
		switch (sortArrow)
		{
			case Direction::Up:
			{
				return wxHDR_SORT_ICON_UP;
			}
			case Direction::Down:
			{
				return wxHDR_SORT_ICON_DOWN;
			}
		};
		return wxHDR_SORT_ICON_NONE;
	}
}

namespace kxf
{
	wxRendererNative& GDIRendererNative::GetRenderer() const
	{
		return wxRendererNative::Get();
	}

	String GDIRendererNative::GetName() const
	{
		return wxS("GDIRendererNative (UxTheme + wxRendererNative)");
	}
	Version GDIRendererNative::GetVersion() const
	{
		return {1, 0};
	}

	// TreeView expander button
	Size GDIRendererNative::GetExpanderButtonSize(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		if (window)
		{
			if (UxTheme theme(const_cast<wxWindow&>(*window), UxThemeClass::TreeView); theme)
			{
				const int partID = widgetFlags.Contains(NativeWidgetFlag::Current) ? TVP_HOTGLYPH : TVP_GLYPH;
				const int stateID = widgetFlags.Contains(NativeWidgetFlag::Expanded) ? GLPS_OPENED : GLPS_CLOSED;

				wxClientDC dc(const_cast<wxWindow*>(window));
				return theme.GetPartSize(dc, partID, stateID);
			}
			else
			{
				// They should be of the same size
				return GetRenderer().GetCheckBoxSize(const_cast<wxWindow*>(window), *MapWidgetFlags(widgetFlags));
			}
		}
		return {};
	}
	void GDIRendererNative::DrawExpanderButton(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (window)
		{
			if (UxTheme theme(const_cast<wxWindow&>(*window), UxThemeClass::TreeView); theme)
			{
				const int partID = widgetFlags.Contains(NativeWidgetFlag::Current) ? TVP_HOTGLYPH : TVP_GLYPH;
				const int stateID = widgetFlags.Contains(NativeWidgetFlag::Expanded) ? GLPS_OPENED : GLPS_CLOSED;

				theme.DrawBackground(dc, partID, stateID, Rect(rect.GetPosition(), theme.GetPartSize(dc, partID, stateID)));
			}
			else
			{
				return GetRenderer().DrawTreeItemButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
			}
		}
	}

	// Progress meter
	void GDIRendererNative::DrawProgressMeter(wxWindow* window, GDIContext& dc, const Rect& rect, int value, int range, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (window)
		{
			// Progress bar looks really ugly when it's smaller than 10x10 pixels, so don't draw it at all in this case.
			const Size minSize = window->FromDIP(wxSize(10, 10));
			if (rect.GetWidth() < minSize.GetWidth() || rect.GetHeight() < minSize.GetHeight())
			{
				return;
			}

			if (UxTheme theme(*window, UxThemeClass::Progress); theme)
			{
				const int bar = widgetFlags.Contains(NativeWidgetFlag::Vertical) ? PP_BARVERT : PP_BAR;
				const int fill = widgetFlags.Contains(NativeWidgetFlag::Vertical) ? PP_FILLVERT : PP_FILL;

				if (widgetFlags.Contains(NativeWidgetFlag::Paused))
				{
					theme.DrawProgressBar(dc, bar, fill, PBFS_PAUSED, rect, value, range);
				}
				else if (widgetFlags.Contains(NativeWidgetFlag::Error))
				{
					theme.DrawProgressBar(dc, bar, fill, PBFS_ERROR, rect, value, range);
				}
				else
				{
					theme.DrawProgressBar(dc, bar, fill, PBFS_NORMAL, rect, value, range);
				}
			}
			else
			{
				GetRenderer().DrawGauge(window, dc.ToWxDC(), rect, value, range, *MapWidgetFlags(widgetFlags));
			}
		}
	}

	// Item selection rectangle
	void GDIRendererNative::DrawItemSelectionRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		if (window)
		{
			// We'd better draw selection rect ourselves using UxTheme because wxWidgets draws windows background first
			// and this causes solid black background being drawn when a graphics renderer is used. The 'DrawItemSelectionRect'
			// function is the only function that does this.
			if (UxTheme theme(*window, UxThemeClass::ListView); theme)
			{
				theme.DrawBackground(dc, LVP_LISTITEM, GetListItemState(widgetFlags), rect);
			}
			else
			{
				GetRenderer().DrawItemSelectionRect(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
			}
		}
	}
	void GDIRendererNative::DrawItemFocusRect(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		GetRenderer().DrawFocusRect(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
	}

	// Title bar button
	void GDIRendererNative::DrawTitleBarButton(wxWindow* window, GDIContext& dc, const Rect& rect, NativeTitleBarButton button, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		auto DoDraw = [&](wxTitleBarButton button)
		{
			GetRenderer().DrawTitleBarBitmap(window, dc.ToWxDC(), rect, button, *MapWidgetFlags(widgetFlags));
		};

		switch (button)
		{
			case NativeTitleBarButton::Help:
			{
				DoDraw(wxTITLEBAR_BUTTON_HELP);
				break;
			}
			case NativeTitleBarButton::Close:
			{
				DoDraw(wxTITLEBAR_BUTTON_CLOSE);
				break;
			}
			case NativeTitleBarButton::Maximize:
			{
				DoDraw(wxTITLEBAR_BUTTON_MAXIMIZE);
				break;
			}
			case NativeTitleBarButton::Minimize:
			{
				DoDraw(wxTITLEBAR_BUTTON_ICONIZE);
				break;
			}
			case NativeTitleBarButton::Restore:
			{
				DoDraw(wxTITLEBAR_BUTTON_RESTORE);
				break;
			}
		};
	}

	// Splitter widget
	NativeSplitterInfo GDIRendererNative::GetSplitterInfo(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		auto info = GetRenderer().GetSplitterParams(window);
		return {info.border, info.widthSash, info.isHotSensitive};
	}

	void GDIRendererNative::DrawSplitterBorder(wxWindow* window, GDIContext& dc, const Rect& rect, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		GetRenderer().DrawSplitterBorder(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags));
	}
	void GDIRendererNative::DrawSplitterSash(wxWindow* window, GDIContext& dc, const Rect& rect, int position, FlagSet<NativeWidgetFlag> widgetFlags)
	{
		GDIAction::Clip clip(dc, rect);
		GetRenderer().DrawSplitterSash(window, dc.ToWxDC(), rect.GetSize(), position, widgetFlags.Contains(NativeWidgetFlag::Vertical) ? wxVERTICAL : wxHORIZONTAL);
	}

	// Header widget
	int GDIRendererNative::GetHeaderButtonHeight(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		return GetRenderer().GetHeaderButtonHeight(const_cast<wxWindow*>(window));
	}
	int GDIRendererNative::GetHeaderButtonMargin(const wxWindow* window, FlagSet<NativeWidgetFlag> widgetFlags) const
	{
		return GetRenderer().GetHeaderButtonMargin(const_cast<wxWindow*>(window));
	}

	int GDIRendererNative::DrawHeaderButton(wxWindow* window,
											GDIContext& dc,
											const Rect& rect,
											FlagSet<NativeWidgetFlag> widgetFlags,
											Direction sortArrow,
											const NativeHeaderButtonInfo* buttonInfo
	)
	{
		if (buttonInfo)
		{
			wxHeaderButtonParams parameters;
			parameters.m_arrowColour = buttonInfo->ArrowColor;
			parameters.m_labelColour = buttonInfo->LabelColor;
			parameters.m_selectionColour = buttonInfo->SelectionColor;

			parameters.m_labelText = buttonInfo->LabelText;
			parameters.m_labelFont = buttonInfo->LabelFont.ToWxFont();
			parameters.m_labelBitmap = buttonInfo->LabelBitmap.ToWxBitmap();
			parameters.m_labelAlignment = buttonInfo->LabelAlignment.ToInt();

			return GetRenderer().DrawHeaderButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags), MapSortArrow(sortArrow), &parameters);
		}
		else
		{
			return GetRenderer().DrawHeaderButton(window, dc.ToWxDC(), rect, *MapWidgetFlags(widgetFlags), MapSortArrow(sortArrow));
		}
	}
	int GDIRendererNative::DrawHeaderButtonContent(wxWindow* window,
												   GDIContext& dc,
												   const Rect& rect,
												   FlagSet<NativeWidgetFlag> widgetFlags,
												   Direction sortArrow,
												   const NativeHeaderButtonInfo* buttonInfo
	)
	{
		return 0;
	}
}
