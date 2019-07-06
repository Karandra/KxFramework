#include "KxStdAfx.h"
#include "ToolTip.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include "KxFramework/KxToolTipEx.h"
#include <wx/textwrapper.h>

namespace KxDataView2
{
	const Column& ToolTip::SelectAnchorColumn(const Column& currentColumn) const
	{
		if (m_AnchorColumn && m_AnchorColumn->IsVisible())
		{
			return *m_AnchorColumn;
		}
		return currentColumn;
	}
	wxPoint ToolTip::GetPopupPosition(const Node& node, const Column& column) const
	{
		const wxRect rect = node.GetClientCellRect(&SelectAnchorColumn(column));
		return rect.GetPosition() + wxPoint(0, rect.GetHeight() + 1);
	}
	wxPoint ToolTip::AdjustPopupPosition(const Node& node, const wxPoint& pos) const
	{
		MainWindow* mainWindow = node.GetMainWindow();

		wxSize screenSize = {wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)};
		wxSize iconSize = {wxSystemSettings::GetMetric(wxSYS_ICON_X), wxSystemSettings::GetMetric(wxSYS_ICON_Y)};
		wxSize textExtent = wxClientDC(mainWindow).GetMultiLineTextExtent(m_Message) + iconSize * 1.5;
		wxSize offset = wxSize(wxSystemSettings::GetMetric(wxSYS_SMALLICON_X), wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y)) / 2;

		wxPoint adjustedPos = mainWindow->ClientToScreen(pos);
		if (int right = adjustedPos.x + textExtent.GetWidth(); right > screenSize.GetWidth())
		{
			adjustedPos.x -= (right - screenSize.GetWidth()) + offset.GetWidth();
		}
		if (int bottom = adjustedPos.y + textExtent.GetHeight(); bottom > screenSize.GetHeight())
		{
			adjustedPos.y -= (bottom - screenSize.GetHeight()) + offset.GetHeight();
		}
		return mainWindow->ScreenToClient(adjustedPos);
	}
	wxString ToolTip::StripMarkupIfNeeded(const Node& node, const Column& column, const wxString& text) const
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
				KxToolTipEx& tooltip = mainWindow->m_ToolTip;
				tooltip.SetCaption(StripMarkupIfNeeded(node, column, m_Caption));
				tooltip.SetMessage(StripMarkupIfNeeded(node, column, m_Message));

				if (auto icon = GetIconBitmap(); icon.IsOk())
				{
					tooltip.SetIcon(icon);
				}
				else
				{
					tooltip.SetIcon(GetIconID());
				}

				const wxPoint pos = AdjustPopupPosition(node, GetPopupPosition(node, column));
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
		if (GetIconID() != KxICON_NONE || GetIconBitmap().IsOk())
		{
			return !m_Caption.IsEmpty() && !m_Message.IsEmpty();
		}
		return !m_Message.IsEmpty();
	}
}
