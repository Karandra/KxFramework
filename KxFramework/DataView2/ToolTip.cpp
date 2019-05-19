#include "KxStdAfx.h"
#include "ToolTip.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include <wx/richtooltip.h>

namespace KxDataView2
{
	bool ToolTip::Show(const Node& node, const Column& column)
	{
		if (MainWindow* mainWindow = node.GetMainWindow())
		{
			if (!m_Caption.IsEmpty())
			{
				wxRichToolTip tooltip(m_Caption, m_Message);
				tooltip.SetIcon(static_cast<int>(m_Icon));
				if (!m_AutoHide)
				{
					tooltip.SetTimeout(0);
				}

				wxRect cellRect = mainWindow->GetView()->GetItemRect(node, &column);
				tooltip.ShowFor(mainWindow, &cellRect);
				return true;
			}
			else
			{
				mainWindow->SetToolTip(m_Message);
				return mainWindow->GetToolTip() != nullptr;
			}
		}
		return false;
	}

	bool ToolTip::IsOK() const
	{
		if (m_Icon != KxICON_NONE)
		{
			return !m_Caption.IsEmpty() && !m_Message.IsEmpty();
		}
		return !m_Message.IsEmpty();
	}
}
