#include "KxStdAfx.h"
#include "KxDataViewTreeNode.h"
#include "KxDataViewMainWindow.h"

//////////////////////////////////////////////////////////////////////////
void KxDataViewTreeNodeData::DeleteChildNode(KxDataViewTreeNode* node)
{
	delete node;
}

//////////////////////////////////////////////////////////////////////////
KxDataViewTreeNode* KxDataViewTreeNode::CreateRootNode(KxDataViewMainWindow* window)
{
	KxDataViewTreeNode* node = new KxDataViewTreeNode(window, NULL, KxDataViewItem());
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

KxDataViewTreeNode::KxDataViewTreeNode(KxDataViewMainWindow* window, KxDataViewTreeNode* parent, const KxDataViewItem& item)
	:m_MainWindow(window), m_ParentNode(parent), m_Item(item)
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

void KxDataViewTreeNode::InsertChild(KxDataViewTreeNode* node, size_t index)
{
	if (!HasBranchData())
	{
		CreateBranchData();
	}

	const Vector& childNodes = m_BranchData->GetChildren();
	m_BranchData->InsertNode(childNodes.begin() + index, node);

	// TODO: insert into sorted array directly in O(log n) instead of resorting in O(n log n).
	Resort(true);

	#if 0
	const Vector& childNodes = m_BranchData->GetChildren();
	if (m_MainWindow->GetSortColumn() >= -1)
	{
		// Insert into sorted array directly
		auto it = std::upper_bound(childNodes.begin(), childNodes.end(), node);
		m_BranchData->InsertNode(it, node);
	}
	else
	{
		// Just insert
		m_BranchData->InsertNode(childNodes.begin() + index, node);
	}
	#endif
}
bool KxDataViewTreeNode::RemoveChild(KxDataViewTreeNode* node)
{
	Vector& childNodes = m_BranchData->GetChildren();

	auto it = std::find(childNodes.begin(), childNodes.end(), node);
	if (it != childNodes.end())
	{
		m_BranchData->DeleteChild(it);
		return true;
	}
	return false;
}
void KxDataViewTreeNode::Resort(bool noRecurse)
{
	if (HasBranchData())
	{
		KxDataViewModel* model = m_MainWindow->GetModel();
		KxDataViewColumn* column = m_MainWindow->GetOwner()->GetSortingColumn();
		if (column || model->HasDefaultCompare())
		{
			bool multiColumnSort = m_MainWindow->GetOwner()->IsMultiColumnSortUsed();
			bool sortAscending = column ? column->IsSortedAscending() : m_MainWindow->IsAscendingSort();

			Vector& childNodes = m_BranchData->GetChildren();
			std::sort(childNodes.begin(), childNodes.end(), [column, model, multiColumnSort, sortAscending]
			(const KxDataViewTreeNode* node1, const KxDataViewTreeNode* node2)
			{
				bool less = model->Compare(node1->GetItem(), node2->GetItem(), column);
				if (!multiColumnSort)
				{
					return sortAscending ? less : !less;
				}
				return less;
			});

			if (!noRecurse)
			{
				for (KxDataViewTreeNode* childNode: childNodes)
				{
					if (childNode->HasChildren())
					{
						childNode->Resort();
					}
				}
			}
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
void KxDataViewTreeNode::ToggleExpanded()
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
	wxCHECK_MSG(node, false, "can't walk NULL node");
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
		m_ParentNode = node;

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
	m_Result++;
	if (node->GetItem() == m_Item)
	{
		return Result::DONE;
	}

	if (node->GetItem() == *m_Iter)
	{
		m_Iter++;
		return Result::CONTINUE;
	}
	else
	{
		m_Result += node->GetSubTreeCount();
		return Result::SKIP_SUBTREE;
	}
}
