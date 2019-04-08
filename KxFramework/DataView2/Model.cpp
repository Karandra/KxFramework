#include "KxStdAfx.h"
#include "Model.h"
#include "MainWindow.h"

namespace KxDataView2
{
	Node& Model::GetRootNode() const
	{
		return m_MainWindow->GetRootNode();
	}
	View* Model::GetView() const
	{
		return m_MainWindow ? m_MainWindow->GetView() : nullptr;
	}

	bool Model::IsEditable(Node& node, const Column& column) const
	{
		return GetEditor(node, column) != nullptr;
	}
	bool Model::IsActivatable(Node& node, const Column& column) const
	{
		return GetRenderer(node, column).IsActivatable();
	}

	Renderer& Model::GetRenderer(const Node& node, const Column& column) const
	{
		return column.GetRenderer();
	}
	Editor* Model::GetEditor(const Node& node, const Column& column) const
	{
		return column.GetEditor();
	}
}

namespace KxDataView2
{
	size_t ListModel::GetItemCount() const
	{
		return GetMainWindow()->GetRootNode().GetChildrenCount();
	}

	Row ListModel::GetRow(const Node& node) const
	{
		return GetMainWindow()->GetRootNode().FindChild(node);
	}
	Node* ListModel::GetNode(Row row) const
	{
		auto& children = GetMainWindow()->GetRootNode().GetChildren();
		if (row < children.size())
		{
			return children[row];
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
