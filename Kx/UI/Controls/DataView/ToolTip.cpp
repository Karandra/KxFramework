#include "stdafx.h"
#include "ToolTip.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include "Kx/UI/Windows/ToolTipEx.h"
#include <wx/textwrapper.h>

using namespace KxFramework::UI;

namespace KxFramework::UI::DataView
{
	const Column& ToolTip::SelectAnchorColumn(const Column& currentColumn) const
	{
		if (m_AnchorColumn && m_AnchorColumn->IsVisible())
		{
			return *m_AnchorColumn;
		}
		return currentColumn;
	}
	const Column& ToolTip::SelectClipTestColumn(const Column& currentColumn) const
	{
		if (m_ClipTestColumn && m_ClipTestColumn->IsVisible())
		{
			return *m_ClipTestColumn;
		}
		return currentColumn;
	}

	Point ToolTip::GetPopupPosition(const Node& node, const Column& column) const
	{
		const Rect rect = node.GetClientCellRect(&SelectAnchorColumn(column));
		return rect.GetPosition() + Point(0, rect.GetHeight() + 1);
	}
	Point ToolTip::AdjustPopupPosition(const Node& node, const Point& pos) const
	{
		MainWindow* mainWindow = node.GetMainWindow();
		const Size screenSize = {wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)};
		const Size smallIcon = {wxSystemSettings::GetMetric(wxSYS_SMALLICON_X), wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y)};

		Size textExtent = wxClientDC(mainWindow).GetMultiLineTextExtent(m_Message);
		Size offset;

		if (auto icon = GetIconBitmap(); icon.IsOk())
		{
			textExtent += icon.GetSize() * 1.5;
			offset = smallIcon / 2;
		}
		else if (auto icon = GetIconID(); icon != StdIcon::None)
		{
			textExtent += smallIcon * 1.5;
			offset = smallIcon / 2;
		}
		else
		{
			offset = smallIcon;
		}

		
		Point adjustedPos = mainWindow->ClientToScreen(pos);
		if (int right = adjustedPos.GetX() + textExtent.GetWidth(); right > screenSize.GetWidth())
		{
			adjustedPos.X() -= (right - screenSize.GetWidth()) + offset.GetWidth();
		}
		if (int bottom = adjustedPos.GetY() + textExtent.GetHeight(); bottom > screenSize.GetHeight())
		{
			adjustedPos.Y() -= (bottom - screenSize.GetHeight()) + offset.GetHeight();
		}
		return mainWindow->ScreenToClient(adjustedPos);
	}
	String ToolTip::StripMarkupIfNeeded(const Node& node, const Column& column, const String& text) const
	{
		const Renderer& renderer = node.GetRenderer(column);
		if (renderer.IsMarkupEnabled())
		{
			return renderer.GetRenderEngine().StripMarkup(text);
		}
		return text;
	}

	bool ToolTip::Show(const Node& node, const Column& column)
	{
		if (MainWindow* mainWindow = node.GetMainWindow())
		{
			if (!m_Caption.IsEmpty())
			{
				ToolTipEx& tooltip = mainWindow->m_ToolTip;
				tooltip.SetCaption(StripMarkupIfNeeded(node, column, m_Caption));
				tooltip.SetMessage(StripMarkupIfNeeded(node, column, m_Message));

				tooltip.RemoveOption(ToolTipExStyle::LargeIcons);
				if (auto icon = GetIconBitmap(); icon.IsOk())
				{
					tooltip.SetIcon(icon);
				}
				else
				{
					tooltip.SetIcon(GetIconID());
				}

				const Point pos = AdjustPopupPosition(node, GetPopupPosition(node, column));
				tooltip.Popup(pos);
				return true;
			}
			else
			{
				mainWindow->SetToolTip(StripMarkupIfNeeded(node, column, m_Message));
				return mainWindow->GetToolTip() != nullptr;
			}
		}
		return false;
	}

	bool ToolTip::IsOK() const
	{
		if (HasAnyIcon())
		{
			return !m_Caption.IsEmpty() && !m_Message.IsEmpty();
		}
		return !m_Message.IsEmpty();
	}
}
