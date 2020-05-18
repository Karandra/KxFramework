#include "stdafx.h"
#include "Node.h"
#include "MainWindow.h"
#include "View.h"

namespace KxFramework::UI::DataView
{
	class Comparator
	{
		private:
			const View* m_View = nullptr;
			const SortOrder m_SortOrder;

		public:
			Comparator(const MainWindow* mainWindow, const SortOrder& sortOrder)
				:m_SortOrder(sortOrder), m_View(mainWindow->GetView())
			{
			}

		public:
			bool operator()(const Node* node1, const Node* node2) const
			{
				const Column* sortingColumn = m_View->GetSortingColumn();
				if (sortingColumn)
				{
					const bool multiColumnSort = m_View->IsMultiColumnSortUsed();
					const bool sortAscending = m_SortOrder.IsAscending();

					const bool isLess = node1->Compare(*node2, *sortingColumn);
					if (!multiColumnSort)
					{
						return sortAscending ? isLess : !isLess;
					}
					return isLess;
				}
				return false;
			}
	};
}

namespace KxFramework::UI::DataView
{
	void Node::PutChildInSortOrder(Node* node)
	{
		// The childNode has changed, and may need to be moved to another location in the sorted child list.
		MainWindow* mainWindow = GetMainWindow();
		if (mainWindow && IsNodeExpanded() && !m_SortOrder.IsNone() && m_Children.size() > 1)
		{
			// We should already be sorted in the right order.
			wxASSERT(m_SortOrder == mainWindow->GetSortOrder());

			// Remove and reinsert the node in the child list
			const Row nodeIndex = FindChild(*node);
			if (nodeIndex)
			{
				m_Children.erase(m_Children.begin() + nodeIndex.GetValue());

				// Use binary search to find the correct position to insert at.
				auto it = std::lower_bound(m_Children.begin(), m_Children.end(), node, Comparator(mainWindow, m_SortOrder));
				m_Children.insert(it, node);
				RecalcIndexes(std::distance(m_Children.begin(), it));

				// Make sure the change is actually shown right away
				mainWindow->UpdateDisplay();
			}
		}
	}
	void Node::Resort(bool force)
	{
		MainWindow* mainWindow = GetMainWindow();
		if (mainWindow && IsNodeExpanded())
		{
			const SortOrder sortOrder = mainWindow->GetSortOrder();
			if (!sortOrder.IsNone())
			{
				// Only sort the children if they aren't already sorted by the wanted criteria.
				if (force || m_SortOrder != sortOrder)
				{
					std::sort(m_Children.begin(), m_Children.end(), Comparator(mainWindow, sortOrder));
					m_SortOrder = sortOrder;
					RecalcIndexes();
				}

				// There may be open child nodes that also need a resort.
				for (Node* childNode: m_Children)
				{
					if (childNode->HasChildren())
					{
						childNode->Resort(force);
					}
				}
			}
		}
	}
	void Node::ChangeSubTreeCount(intptr_t num)
	{
		if (m_IsExpanded)
		{
			m_SubTreeCount += num;
			if (m_ParentNode)
			{
				m_ParentNode->ChangeSubTreeCount(num);
			}
		}
	}
	intptr_t Node::ToggleNodeExpanded()
	{
		// We do not allow the (invisible) root node to be collapsed because there is no way to expand it again.
		if (!IsRootNode())
		{
			intptr_t count = 0;
			for (const Node* node: m_Children)
			{
				count += node->GetSubTreeCount() + 1;
			}

			if (m_IsExpanded)
			{
				ChangeSubTreeCount(-count);
				m_IsExpanded = false;
			}
			else
			{
				m_IsExpanded = true;
				ChangeSubTreeCount(+count);

				// Sort the children if needed
				Resort();
			}
		}
		return m_SubTreeCount;
	}

	void Node::InitNodeUsing(const Node& node)
	{
		m_ParentNode = node.m_ParentNode;
		m_RootNode = node.m_RootNode;
		m_SortOrder = node.m_SortOrder;
	}
	void Node::RecalcIndexes(size_t startAt)
	{
		for (size_t i = startAt; i < m_Children.size(); ++i)
		{
			m_Children[i]->m_IndexWithinParent = i;
		}
	}

	Node::~Node()
	{
		DetachAllChildren();
	}

	Row Node::FindChild(const Node& node) const
	{
		if (Row index = node.GetIndexWithinParent(); index && index < m_Children.size())
		{
			return index;
		}
		return {};
	}
	int Node::GetIndentLevel() const
	{
		if (!IsRootNode())
		{
			int level = 0;

			const Node* node = this;
			while (node->GetParent()->GetParent())
			{
				node = node->GetParent();
				level++;
			}
			return level;
		}
		return -1;
	}

	void Node::AttachChild(Node& node, size_t index)
	{
		// Flag indicating whether we should retain existing sorted list when inserting the child node.
		bool shouldInsertSorted = false;
		SortOrder controlSortOrder = m_RootNode->GetMainWindow()->GetSortOrder();

		if (controlSortOrder.IsNone())
		{
			// We should insert assuming an unsorted list. This will cause the child list to lose the current sort order, if any.
			ResetSortOrder();
		}
		else if (!HasChildren())
		{
			if (IsNodeExpanded())
			{
				// We don't need to search for the right place to insert the first item (there is only one),
				// but we do need to remember the sort order to use for the subsequent ones.
				m_SortOrder = controlSortOrder;
			}
			else
			{
				// We're inserting the first child of a closed node. We can choose whether to consider this empty
				// child list sorted or unsorted. By choosing unsorted, we postpone comparisons until the parent
				// node is opened in the view, which may be never.
				ResetSortOrder();
			}
		}
		else if (IsNodeExpanded())
		{
			// For open branches, children should be already sorted.
			wxASSERT_MSG(m_SortOrder == controlSortOrder, wxS("Logic error in DataView2 sorting code"));

			// We can use fast insertion.
			shouldInsertSorted = true;
		}
		else if (m_SortOrder == controlSortOrder)
		{
			// The children are already sorted by the correct criteria (because the node must have been opened
			// in the same time in the past). Even though it is closed now, we still insert in sort order to
			// avoid a later resort.
			shouldInsertSorted = true;
		}
		else
		{
			// The children of this closed node aren't sorted by the correct criteria, so we just insert unsorted.
			ResetSortOrder();
		}

		// Copy attributes and set this node as parent for child node
		node.InitNodeUsing(*this);
		node.m_ParentNode = this;

		const intptr_t addedCount = node.GetSubTreeCount() + 1;
		if (shouldInsertSorted)
		{
			// Use binary search to find the correct position to insert at.
			auto it = std::lower_bound(m_Children.begin(), m_Children.end(), &node, Comparator(m_RootNode->GetMainWindow(), controlSortOrder));
			m_Children.insert(it, &node);
			RecalcIndexes(std::distance(m_Children.begin(), it));
		}
		else
		{
			index = std::min(index, m_Children.size());
			m_Children.insert(m_Children.begin() + index, &node);
			RecalcIndexes(index);
		}

		m_RootNode->GetMainWindow()->OnNodeAdded(*this);
		ChangeSubTreeCount(+addedCount);
	}
	Node* Node::DetachChild(size_t index)
	{
		if (index < m_Children.size())
		{
			auto it = m_Children.begin() + index;
			Node* node = *it;
			node->m_ParentNode = nullptr;
			m_Children.erase(it);
			RecalcIndexes(index);

			const intptr_t removedCount = node->GetSubTreeCount() + 1;
			if (MainWindow* mainWindow = GetMainWindow())
			{
				mainWindow->OnNodeRemoved(*node, removedCount);
			}
			ChangeSubTreeCount(-removedCount);
			return node;
		}
		return nullptr;
	}
	Node* Node::DetachChild(Node& node)
	{
		if (Row index = FindChild(node))
		{
			return DetachChild(index.GetValue());
		}
		return nullptr;
	}
	void Node::DetachAllChildren()
	{
		const intptr_t removedCount = GetSubTreeCount();
		ChangeSubTreeCount(-removedCount);

		for (Node* node: m_Children)
		{
			node->m_ParentNode = nullptr;
		}
		m_Children.clear();
	}
	
	bool Node::Swap(Node& otherNode)
	{
		if (this != &otherNode && m_ParentNode == otherNode.GetParent())
		{
			auto it = m_Children.begin() + m_IndexWithinParent.GetValue();
			auto otherIt = otherNode.m_Children.begin() + otherNode.m_IndexWithinParent.GetValue();
			std::iter_swap(it, otherIt);
			return true;
		}
		return false;
	}
}

namespace KxFramework::UI::DataView
{
	MainWindow* Node::GetMainWindow() const
	{
		return m_RootNode ? m_RootNode->GetMainWindow() : nullptr;
	}
	View* Node::GetView() const
	{
		return m_RootNode ? m_RootNode->GetView() : nullptr;
	}
	Model* Node::GetModel() const
	{
		return m_RootNode ? m_RootNode->GetModel() : nullptr;
	}
	bool Node::IsRenderable(const Column& column) const
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			return &GetRenderer(column) != &mainWindow->GetNullRenderer();
		}
		return false;
	}

	bool Node::IsExpanded() const
	{
		return IsNodeExpanded() && HasChildren();
	}
	void Node::SetExpanded(bool expand)
	{
		if (View* view = GetView())
		{
			if (expand)
			{
				view->Expand(*this);
			}
			else
			{
				view->Collapse(*this);
			}
		}
	}
	void Node::Expand()
	{
		if (View* view = GetView())
		{
			view->Expand(*this);
		}
	}
	void Node::Collapse()
	{
		if (View* view = GetView())
		{
			view->Collapse(*this);
		}
	}
	void Node::ToggleExpanded()
	{
		SetExpanded(!IsExpanded());
	}

	void Node::Refresh()
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			mainWindow->OnCellChanged(*this, nullptr);
		}
	}
	void Node::Refresh(Column& column)
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			mainWindow->OnCellChanged(*this, &column);
		}
	}
	void Node::Edit(Column& column)
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			mainWindow->BeginEdit(*this, column);
		}
	}

	Row Node::GetRow() const
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			return mainWindow->GetRowByNode(*this);
		}
		return {};
	}
	bool Node::IsSelected() const
	{
		if (View* view = GetView())
		{
			return view->IsSelected(*this);
		}
		return false;
	}
	bool Node::IsCurrent() const
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			if (mainWindow->IsVirtualList())
			{
				return GetRow() == mainWindow->m_CurrentRow;
			}
			else
			{
				return mainWindow->GetView()->GetCurrentItem() == this;
			}
		}
		return false;
	}
	bool Node::IsHotTracked() const
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			if (mainWindow->IsVirtualList())
			{
				return GetRow() == mainWindow->m_HotTrackRow;
			}
			else
			{
				return mainWindow->GetView()->GetHotTrackedItem() == this;
			}
		}
		return false;
	}
	void Node::SetSelected(bool value)
	{
		if (View* view = GetView())
		{
			if (value)
			{
				view->Select(*this);
			}
			else
			{
				view->Unselect(*this);
			}
		}
	}
	void Node::EnsureVisible(const Column* column)
	{
		if (View* view = GetView())
		{
			view->EnsureVisible(*this, column);
		}
	}

	wxRect Node::GetCellRect(const Column* column) const
	{
		if (View* view = GetView())
		{
			return view->GetAdjustedItemRect(*this, column);
		}
		return {};
	}
	wxRect Node::GetClientCellRect(const Column* column) const
	{
		if (View* view = GetView())
		{
			return view->GetItemRect(*this, column);
		}
		return {};
	}
	wxPoint Node::GetDropdownMenuPosition(const Column* column) const
	{
		if (View* view = GetView())
		{
			return view->GetDropdownMenuPosition(*this, column);
		}
		return {};
	}
}

namespace KxFramework::UI::DataView
{
	bool Node::IsEditable(const Column& column) const
	{
		return IsEnabled(column) && GetEditor(column) != nullptr;
	}
	bool Node::IsActivatable(const Column& column) const
	{
		return IsEnabled(column) && GetRenderer(column).IsActivatable();
	}

	Renderer& Node::GetRenderer(const Column& column) const
	{
		return GetModel()->GetRenderer(*this, column);
	}
	Editor* Node::GetEditor(const Column& column) const
	{
		return GetModel()->GetEditor(*this, column);
	}
	bool Node::IsEnabled(const Column& column) const
	{
		return GetModel()->IsEnabled(*this, column);
	}

	wxAny Node::GetValue(const Column& column) const
	{
		return GetModel()->GetValue(*this, column);
	}
	wxAny Node::GetEditorValue(const Column& column) const
	{
		return GetModel()->GetEditorValue(*this, column);
	}
	ToolTip Node::GetToolTip(const Column& column) const
	{
		return GetModel()->GetToolTip(*this, column);
	}
	bool Node::SetValue(Column& column, const wxAny& value)
	{
		return GetModel()->SetValue(*this, column, value);
	}

	bool Node::GetAttributes(const Column& column, const CellState& cellState, CellAttribute& attributes) const
	{
		return GetModel()->GetAttributes(*this, column, cellState, attributes);
	}
	bool Node::IsCategoryNode() const
	{
		return GetModel()->IsCategoryNode(*this);
	}
	int Node::GetRowHeight() const
	{
		return GetModel()->GetRowHeight(*this);
	}

	bool Node::Compare(const Node& other, const Column& column) const
	{
		return GetModel()->Compare(*this, other, column);
	}
}

namespace KxFramework::UI::DataView
{
	bool NodeOperation::DoWalk(Node& node, NodeOperation& func)
	{
		switch (func(node))
		{
			case Result::Done:
			{
				return true;
			}
			case Result::SkipSubTree:
			{
				return false;
			}
			case Result::Continue:
			{
				break;
			}
		};

		if (node.HasChildren())
		{
			for (Node* childNode: node.GetChildren())
			{
				if (DoWalk(*childNode, func))
				{
					return true;
				}
			}
		}
		return false;
	}
}

namespace KxFramework::UI::DataView
{
	void RootNode::ResetAll()
	{
		Node::ResetAll();
		Init();
	}

	View* RootNode::GetView() const
	{
		return m_MainWindow ? m_MainWindow->GetView() : nullptr;
	}
	Model* RootNode::GetModel() const
	{
		return m_MainWindow ? m_MainWindow->GetModel() : nullptr;
	}
}

namespace KxFramework::UI::DataView
{
	NodeOperation::Result NodeOperation_RowToNode::operator()(Node& node)
	{
		m_CurrentRow++;
		if (m_CurrentRow == m_Row)
		{
			m_ResultNode = &node;
			return Result::Done;
		}

		if (node.GetSubTreeCount() + m_CurrentRow < m_Row)
		{
			m_CurrentRow += node.GetSubTreeCount();
			return Result::SkipSubTree;
		}
		else
		{
			// If the current node has only leaf children, we can find the desired node directly.
			// This can speed up finding the node in some cases, and will have a very good effect for list views.
			if (node.HasChildren() && node.GetChildren().size() == (size_t)node.GetSubTreeCount())
			{
				const size_t index = m_Row - m_CurrentRow - 1;
				if (index < node.GetChildrenCount())
				{
					m_ResultNode = node.GetChildren()[index];
					return Result::Done;
				}
			}
			return Result::Continue;
		}
	}
}
