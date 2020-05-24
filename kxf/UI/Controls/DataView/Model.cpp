#include "stdafx.h"
#include "Model.h"
#include "MainWindow.h"

namespace kxf::UI::DataView
{
	Node& Model::GetRootNode() const
	{
		return m_MainWindow->GetRootNode();
	}
	View* Model::GetView() const
	{
		return m_MainWindow ? m_MainWindow->GetView() : nullptr;
	}
	void Model::ItemsChanged()
	{
		if (m_MainWindow)
		{
			m_MainWindow->ItemsChanged();
		}
	}

	bool Model::IsEditable(Node& node, const Column& column) const
	{
		return IsEnabled(node, column) && GetEditor(node, column) != nullptr;
	}
	bool Model::IsActivatable(Node& node, const Column& column) const
	{
		return IsEnabled(node, column) && GetRenderer(node, column).IsActivatable();
	}

	Renderer& Model::GetRenderer(const Node& node, const Column& column) const
	{
		return column.GetRenderer();
	}
	Editor* Model::GetEditor(const Node& node, const Column& column) const
	{
		return column.GetEditor();
	}
	ToolTip Model::GetToolTip(const Node& node, const Column& column) const
	{
		return GetRenderer(node, column).CreateToolTip();
	}
}

namespace kxf::UI::DataView
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
			return children[*row];
		}
		return nullptr;
	}
}

namespace kxf::UI::DataView
{
	VirtualNode& VirtualListModel::GetVirtualNode() const
	{
		return GetMainWindow()->m_VirtualNode;
	}

	void VirtualListModel::OnRowInserted(Row row)
	{
		VirtualNode::VirtualRowChanger changeRow(GetVirtualNode(), row);
		GetMainWindow()->OnNodeAdded(changeRow.GetNode());
	}
	void VirtualListModel::OnRowRemoved(Row row)
	{
		VirtualNode::VirtualRowChanger changeRow(GetVirtualNode(), row);
		GetMainWindow()->OnNodeRemoved(changeRow.GetNode(), 1);
	}
}
