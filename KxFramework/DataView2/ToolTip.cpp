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
				tooltip.SetCaption(m_Caption);
				tooltip.SetMessage(m_Message);
				tooltip.SetIcon(m_Icon);

				wxRect rect = mainWindow->GetItemRect(node, &column);
				tooltip.Popup(rect.GetPosition() + wxPoint(0 , rect.GetHeight() + 1));
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
