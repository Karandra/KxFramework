#include "KxStdAfx.h"
#include "KxDataViewTreeNode.h"
#include "KxDataViewMainWindow.h"
#include "KxDataViewCtrl.h"

//////////////////////////////////////////////////////////////////////////
class KxDataViewComparator
{
	private:
		KxDataViewSortOrder m_SortOrder;
		KxDataViewMainWindow* m_MainWindow = nullptr;
		KxDataViewColumn* m_SortingColumn = nullptr;
		KxDataViewModel* m_DataModel = nullptr;

	public:
		KxDataViewComparator(KxDataViewMainWindow* window, const KxDataViewSortOrder& sortOrder)
			:m_SortOrder(sortOrder), m_MainWindow(window), m_DataModel(window->GetModel())
		{
		}

	public:
		bool operator()(KxDataViewTreeNode* node1, KxDataViewTreeNode* node2) const
		{
			KxDataViewCtrl* dataView = m_MainWindow->GetOwner();
			KxDataViewColumn* sortingColumn = dataView->GetSortingColumn();
			if (sortingColumn || m_DataModel->HasDefaultCompare())
			{
				const bool multiColumnSort = dataView->IsMultiColumnSortUsed();
				const bool sortAscending = m_SortOrder.IsAscending();

				const bool isLess = m_DataModel->Compare(node1->GetItem(), node2->GetItem(), sortingColumn);
				if (!multiColumnSort)
				{
					return sortAscending ? isLess : !isLess;
				}
				return isLess;
			}
		}
};

//////////////////////////////////////////////////////////////////////////
void KxDataViewTreeNodeData::DeleteChildNode(KxDataViewTreeNode* node)
{
	delete node;
}

//////////////////////////////////////////////////////////////////////////
KxDataViewTreeNode* KxDataViewTreeNode::CreateRootNode(KxDataViewMainWindow* window)
{
	KxDataViewTreeNode* node = new KxDataViewTreeRootNode(window, nullptr, KxDataViewItem());
	node->CreateBranchData(true);
	return node;
}
bool KxDataViewTreeNode::SwapNodes(KxDataViewTreeNode* node1, KxDataViewTreeNode* node2)
{
	if (node1 != node2 && (node1->HasParent() && node2->HasParent()) && node1->GetParent() == node2->GetParent())
	{
		Vector& nodesStore = node1->GetParent()->GetChildNodes();

		auto it1 = std::find(nodesStore.begin(), nodesStore.end(), node1);
		auto it2 = std::find(nodesStore.begin(), nodesStore.end(), node2);
		std::iter_swap(it1, it2);
		return true;
	}
	return false;
}

bool KxDataViewTreeNode::HasBranchData() const
{
	return m_BranchData.has_value();
}
void KxDataViewTreeNode::CreateBranchData(bool isExpanded)
{
	m_BranchData.emplace(isExpanded);
}
void KxDataViewTreeNode::DestroyBranchData()
{
	m_BranchData.reset();
}

KxDataViewTreeNode::KxDataViewTreeNode(KxDataViewTreeNode* parent, const KxDataViewItem& item)
	:m_ParentNode(parent), m_Item(item)
{
}
KxDataViewTreeNode::~KxDataViewTreeNode()
{
}

KxDataViewTreeNode::Vector& KxDataViewTreeNode::GetChildNodes()
{
	wxASSERT(HasBranchData());
	return m_BranchData->GetChildren();
}
const KxDataViewTreeNode::KxDataViewTreeNode::Vector& KxDataViewTreeNode::GetChildNodes() const
{
	wxASSERT(HasBranchData());
	return m_BranchData->GetChildren();
}
size_t KxDataViewTreeNode::GetChildNodesCount() const
{
	if (HasBranchData())
	{
		return m_BranchData->GetChildrenCount();
	}
	return 0;
}

void KxDataViewTreeNode::InsertChild(KxDataViewMainWindow* window, KxDataViewTreeNode* node, size_t index)
{
	if (!HasBranchData())
	{
		CreateBranchData();
	}

	const KxDataViewSortOrder sortOrder = window->GetSortOrder();

	// Flag indicating whether we should retain existing sorted list when inserting the child node.
	bool shouldInsertSorted = false;

	if (sortOrder.IsNone())
	{
		// We should insert assuming an unsorted list. This will cause the child list to lose the current sort order, if any.
		m_BranchData->ResetSortOrder();
	}
	else if (!m_BranchData->HasChildren())
	{
		if (m_BranchData->IsExpanded())
		{
			// We don't need to search for the right place to insert the first item (there is only one),
			// but we do need to remember the sort order to use for the subsequent ones.
			m_BranchData->SetSortOrder(sortOrder);
		}
		else
		{
			// We're inserting the first child of a closed node. We can choose whether to consider this empty
			// child list sorted or unsorted. By choosing unsorted, we postpone comparisons until the parent
			// node is opened in the view, which may be never.
			m_BranchData->ResetSortOrder();
		}
	}
	else if (m_BranchData->IsExpanded())
	{
		// For open branches, children should be already sorted.
		wxASSERT_MSG(m_branchData->sortOrder == sortOrder, wxS("Logic error in KxDVC sorting code"));

		// We can use fast insertion.
		shouldInsertSorted = true;
	}
	else if (m_BranchData->GetSortOrder() == sortOrder)
	{
		// The children are already sorted by the correct criteria (because the node must have been opened
		// in the same time in the past). Even though it is closed now, we still insert in sort order to
		// avoid a later resort.
		shouldInsertSorted = true;
	}
	else
	{
		// The children of this closed node aren't sorted by the correct criteria, so we just insert unsorted.
		m_BranchData->ResetSortOrder();
	}

	if (shouldInsertSorted)
	{
		// Use binary search to find the correct position to insert at.
		Vector& children = m_BranchData->GetChildren();
		auto it = std::lower_bound(children.begin(), children.end(), node, KxDataViewComparator(window, sortOrder));
		m_BranchData->InsertChild(node, it);

		#if 0
		// Use binary search to find the correct position to insert at.
		size_t low = 0;
		size_t high = m_BranchData->GetChildrenCount();

		KxDataViewComparator comparator(window, sortOrder);
		while (low < high)
		{
			const size_t mid = low + (high - low) / 2;
			if (comparator(m_BranchData->GetChildren()[mid], node))
			{
				low = mid + 1;
			}
			else
			{
				high = mid - 1;
			}
		}
		m_BranchData->InsertChild(node, low);
		#endif
	}
	else
	{
		m_BranchData->InsertChild(node, index);
	}
}
void KxDataViewTreeNode::RemoveChild(size_t index)
{
	m_BranchData->RemoveChild(index);
}
void KxDataViewTreeNode::Resort(KxDataViewMainWindow* window)
{
	if (HasBranchData() && m_BranchData->IsExpanded())
	{
		const KxDataViewSortOrder sortOrder = window->GetSortOrder();
		if (!sortOrder.IsNone())
		{
			Vector& children = m_BranchData->GetChildren();

			// Only sort the children if they aren't already sorted by the wanted criteria.
			if (m_BranchData->GetSortOrder() != sortOrder)
			{
				std::sort(children.begin(), children.end(), KxDataViewComparator(window, sortOrder));
				m_BranchData->SetSortOrder(sortOrder);
			}

			// There may be open child nodes that also need a resort.
			for (KxDataViewTreeNode* childNode: children)
			{
				if (childNode->HasChildren())
				{
					childNode->Resort(window);
				}
			}
		}
	}
}
void KxDataViewTreeNode::PutChildInSortOrder(KxDataViewMainWindow* window, KxDataViewTreeNode* node)
{
	// The childNode has changed, and may need to be moved to another location in the sorted child list.
	if (HasBranchData() && m_BranchData->IsExpanded() && !m_BranchData->GetSortOrder().IsNone())
	{
		Vector& children = m_BranchData->GetChildren();

		// This is more than an optimization, the code below assumes that 1 is a valid index.
		if (children.size() > 1)
		{
			// We should already be sorted in the right order.
			wxASSERT(m_BranchData->GetSortOrder() == window->GetSortOrder());

			// First find the node in the current child list
			intptr_t oldLocation = -1;
			for (size_t index = 0; index < children.size(); ++index)
			{
				if (children[index] == node)
				{
					oldLocation = index;
					break;
				}
			}
			if (oldLocation < 0)
			{
				// Not our child?
				return;
			}

			// Check if we actually need to move the node.
			KxDataViewComparator comparator(window, m_BranchData->GetSortOrder());
			bool locationChanged = false;

			if (oldLocation == 0)
			{
				// Compare with the next item (as we return early in the case of only a single child,
				// we know that there is one) to check if the item is now out of order.
				if (!comparator(node, children[1]))
				{
					locationChanged = true;
				}
			}
			else
			{
				// Compare with the previous item.
				if (!comparator(children[oldLocation - 1], node))
				{
					locationChanged = true;
				}
			}
			if (!locationChanged)
			{
				return;
			}

			// Remove and reinsert the node in the child list
			m_BranchData->RemoveChild(oldLocation);

			// Use binary search to find the correct position to insert at.
			auto it = std::lower_bound(children.begin(), children.end(), node, comparator);
			m_BranchData->InsertChild(node, it);

			#if 0
			size_t high = children.size();
			size_t low = 0;
			while (low < high)
			{
				const size_t mid = low + (high - low) / 2;
				if (comparator(m_BranchData->GetChildren()[mid], node))
				{
					low = mid + 1;
				}
				else
				{
					high = mid - 1;
				}
			}
			m_BranchData->InsertChild(node, low);
			#endif

			// Make sure the change is actually shown right away
			window->UpdateDisplay();
		}
	}
}

ptrdiff_t KxDataViewTreeNode::FindChildByItem(const KxDataViewItem& item) const
{
	if (m_BranchData)
	{
		const Vector& items = m_BranchData->GetChildren();
		auto it = std::find_if(items.begin(), items.end(), [&item](const KxDataViewTreeNode* node)
		{
			return node->GetItem() == item;
		});
		if (it != items.end())
		{
			return std::distance(items.begin(), it);
		}
	}
	return wxNOT_FOUND;
}
int KxDataViewTreeNode::GetIndentLevel() const
{
	int level = 0;
	const KxDataViewTreeNode* node = this;
	while (node->GetParent()->GetParent())
	{
		node = node->GetParent();
		level++;
	}
	return level;
}

bool KxDataViewTreeNode::IsExpanded() const
{
	return HasBranchData() && m_BranchData->IsExpanded();
}
void KxDataViewTreeNode::ToggleExpanded(KxDataViewMainWindow* window)
{
	// We do not allow the (invisible) root node to be collapsed because
	// there is no way to expand it again.
	if (!IsRootNode())
	{
		wxCHECK_RET(HasBranchData(), "Can't open leaf node");

		ptrdiff_t sum = 0;
		for (const KxDataViewTreeNode* node: m_BranchData->GetChildren())
		{
			sum += node->GetSubTreeCount() + 1;
		}

		if (m_BranchData->IsExpanded())
		{
			ChangeSubTreeCount(-sum);
			m_BranchData->ToggleExpanded();
		}
		else
		{
			m_BranchData->ToggleExpanded();
			ChangeSubTreeCount(+sum);

			// Sort the children if needed
			Resort(window);
		}
	}
}

void KxDataViewTreeNode::SetHasChildren(bool hasChildren)
{
	// The invisible root item always has children, so ignore any attempts to change this.
	if (!IsRootNode())
	{
		if (!hasChildren)
		{
			DestroyBranchData();
		}
		else if (!HasBranchData())
		{
			CreateBranchData();
		}
	}
}

ptrdiff_t KxDataViewTreeNode::GetSubTreeCount() const
{
	return HasBranchData() ? m_BranchData->GetSubTreeCount() : 0;
}
void KxDataViewTreeNode::ChangeSubTreeCount(ptrdiff_t num)
{
	wxASSERT(m_BranchData.has_value());
	if (!m_BranchData->IsExpanded())
	{
		return;
	}

	m_BranchData->GetSubTreeCountRef() += num;
	wxASSERT(m_BranchData->GetSubTreeCount() >= 0);

	if (m_ParentNode)
	{
		m_ParentNode->ChangeSubTreeCount(num);
	}
}

//////////////////////////////////////////////////////////////////////////
bool KxDataViewTreeNodeOperation::DoWalk(KxDataViewTreeNode* node, KxDataViewTreeNodeOperation& func)
{
	wxCHECK_MSG(node, false, "can't walk nullptr node");
	switch (func(node))
	{
		case Result::DONE:
		{
			return true;
		}
		case Result::SKIP_SUBTREE:
		{
			return false;
		}
		case Result::CONTINUE:
		{
			break;
		}
	};

	if (node->HasChildren())
	{
		for (KxDataViewTreeNode* childNode: node->GetChildNodes())
		{
			if (DoWalk(childNode, func))
			{
				return true;
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
KxDataViewTreeNodeOperation::Result KxDataViewTreeNodeOperation_RowToTreeNode::operator()(KxDataViewTreeNode* node)
{
	m_CurrentRow++;
	if (m_CurrentRow == m_Row)
	{
		m_ResultNode = node;
		return Result::DONE;
	}

	if (node->GetSubTreeCount() + m_CurrentRow < m_Row)
	{
		m_CurrentRow += node->GetSubTreeCount();
		return Result::SKIP_SUBTREE;
	}
	else
	{
		// If the current node has only leaf children, we can find the
		// desired node directly. This can speed up finding the node
		// in some cases, and will have a very good effect for list views.
		if (node->HasChildren() && node->GetChildNodes().size() == (size_t)node->GetSubTreeCount())
		{
			const size_t index = m_Row - m_CurrentRow - 1;
			if (index < node->GetChildNodes().size())
			{
				m_ResultNode = node->GetChildNodes()[index];
				return Result::DONE;
			}
		}

		return Result::CONTINUE;
	}
}

//////////////////////////////////////////////////////////////////////////
KxDataViewTreeNodeOperation::Result KxDataViewTreeNodeOperation_ItemToRow::operator()(KxDataViewTreeNode* node)
{
	if (node->GetItem() == m_Item)
	{
		return Result::DONE;
	}

	// Is this node the next (grand)parent of the item we're looking for?
	if (node->GetItem() == *m_Iterator)
	{
		++m_Iterator;
		++m_Current;
		return Result::CONTINUE;
	}
	else
	{
		// Skip this node and all its currently visible children.
		m_Current += node->GetSubTreeCount() + 1;
		return Result::SKIP_SUBTREE;
	}
}
