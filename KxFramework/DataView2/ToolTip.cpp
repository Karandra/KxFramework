#include "KxStdAfx.h"
#include "ToolTip.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include "KxFramework/KxToolTipEx.h"

namespace KxDataView2
{
	bool ToolTip::Show(const Node& node, const Column& column)
	{
		if (MainWindow* mainWindow = node.GetMainWindow())
		{
			if (!m_Caption.IsEmpty())
			{
				KxToolTipEx& tooltip = mainWindow->m_ToolTip;
				tooltip.SetCaption(ProcessText(node, column, m_Caption));
				tooltip.SetMessage(ProcessText(node, column, m_Message));

				if (auto icon = GetIconBitmap(); icon.IsOk())
				{
					tooltip.SetIcon(icon);
				}
				else
				{
					tooltip.SetIcon(GetIconID());
				}

				const wxRect rect = mainWindow->GetItemRect(node, &column);
				tooltip.Popup(rect.GetPosition() + wxPoint(0 , rect.GetHeight() + 1));
				return true;
			}
			else
			{
				mainWindow->SetToolTip(ProcessText(node, column, m_Message));
				return mainWindow->GetToolTip() != nullptr;
			}
		}
		return false;
	}
	wxString ToolTip::ProcessText(const Node& node, const Column& column, const wxString& text) const
	{
		const Renderer& renderer = node.GetRenderer(column);
		if (renderer.IsMarkupEnabled())
		{
			return renderer.GetRenderEngine().StripMarkup(text);
		}
		return text;
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
