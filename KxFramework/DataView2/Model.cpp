#include "KxStdAfx.h"
#include "Model.h"
#include "MainWindow.h"

namespace KxDataView2
{
	View* Model::GetView() const
	{
		return m_MainWindow ? m_MainWindow->GetView() : nullptr;
	}
}

namespace KxDataView2
{
	Row ListModel::GetRow(const Node& node) const
	{
		return GetMainWindow()->GetRootNode().FindChild(node);
	}
	Node* ListModel::GetNode(Row row) const
	{
		Node& rootNode = GetMainWindow()->GetRootNode();
		if (rootNode.HasChildren())
		{
			return rootNode.GetChildren()[row.GetValue()];
		}
		return nullptr;
	}
}

namespace KxDataView2
{
	void VirtualListModel::OnRowInserted(Row row)
	{
		if (Node* node = GetNode(row))
		{
			GetMainWindow()->OnNodeAdded(*node);
		}
	}
	void VirtualListModel::OnRowRemoved(Row row)
	{
		if (Node* node = GetNode(row))
		{
			GetMainWindow()->OnNodeRemoved(*node, 1);
		}
	}
}
