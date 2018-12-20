#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/DataView/KxDataViewItem.h"
#include "KxFramework/DataView/KxDataViewSortOrder.h"
#include <optional>
class KX_API KxDataViewMainWindow;
class KX_API KxDataViewTreeNode;

class KX_API KxDataViewTreeNodeData
{
	public:
		using Vector = std::vector<KxDataViewTreeNode*>;

	private:
		// Child nodes. Note that this may be empty in case
		// this branch of the tree wasn't expanded and realized yet.
		Vector m_Children;

		// Order in which children are sorted (possibly none).
		KxDataViewSortOrder m_SortOrder;

		// Total count of expanded (i.e. visible with the help of some
		// scrolling) items in the subtree, but excluding this node. I.e. it is
		// 0 for leaves and is the number of rows the subtree occupies for
		// branch nodes.
		ptrdiff_t m_SubTreeCount = 0;

		// Is the branch node currently expanded?
		bool m_IsExpanded = false;

	private:
		void DeleteChildNode(KxDataViewTreeNode* node);

	public:
		KxDataViewTreeNodeData(bool isExpanded = false)
			:m_SortOrder(KxDataViewSortOrder::UseNone()), m_IsExpanded(isExpanded)
		{
		}
		~KxDataViewTreeNodeData()
		{
			for (KxDataViewTreeNode* node: m_Children)
			{
				DeleteChildNode(node);
			}
		}

	public:
		ptrdiff_t GetSubTreeCount() const
		{
			return m_SubTreeCount;
		}
		ptrdiff_t& GetSubTreeCountRef()
		{
			return m_SubTreeCount;
		}

		bool IsExpanded() const
		{
			return m_IsExpanded;
		}
		void SetExpanded(bool expanded)
		{
			m_IsExpanded = expanded;
		}
		void ToggleExpanded()
		{
			SetExpanded(!IsExpanded());
		}

		const Vector& GetChildren() const
		{
			return m_Children;
		}
		Vector& GetChildren()
		{
			return m_Children;
		}
		bool HasChildren() const
		{
			return !m_Children.empty();
		}
		size_t GetChildrenCount() const
		{
			return m_Children.size();
		}

		void RemoveChild(size_t index)
		{
			auto it = m_Children.begin() + index;
			DeleteChildNode(*it);
			m_Children.erase(it);
		}
		void InsertChild(KxDataViewTreeNode* node, size_t index)
		{
			m_Children.insert(m_Children.begin() + index, node);
		}
		void InsertChild(KxDataViewTreeNode* node, Vector::const_iterator it)
		{
			m_Children.insert(it, node);
		}

		const KxDataViewSortOrder& GetSortOrder() const
		{
			return m_SortOrder;
		}
		void SetSortOrder(const KxDataViewSortOrder& sortOrder)
		{
			m_SortOrder = sortOrder;
		}
		void ResetSortOrder()
		{
			m_SortOrder = KxDataViewSortOrder::UseNone();
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxDataViewTreeNode
{
	public:
		using Vector = std::vector<KxDataViewTreeNode*>;

		static KxDataViewTreeNode* CreateRootNode(KxDataViewMainWindow* window);
		static bool SwapNodes(KxDataViewTreeNode* node1, KxDataViewTreeNode* node2);

	private:
		KxDataViewTreeNode* m_ParentNode = nullptr;
		KxDataViewItem m_Item;

		// Data specific to non-leaf (branch, inner) nodes
		std::optional<KxDataViewTreeNodeData> m_BranchData;

	private:
		bool HasBranchData() const;
		void CreateBranchData(bool isExpanded = false);
		void DestroyBranchData();

		// Called by the child after it has been updated to put it in the right
		// place among its siblings, depending on the sort order.
		//
		// The argument must be non-null, but is passed as a pointer as it's
		// inserted into m_branchData->children.
		void PutChildInSortOrder(KxDataViewMainWindow* window, KxDataViewTreeNode* childNode);

	public:
		KxDataViewTreeNode(KxDataViewTreeNode* parent, const KxDataViewItem& item);
		virtual ~KxDataViewTreeNode();

	public:
		bool IsRootNode()
		{
			return m_ParentNode == nullptr;
		}
		bool HasParent() const
		{
			return m_ParentNode != nullptr;
		}
		
		KxDataViewTreeNode* GetParent() const
		{
			return m_ParentNode;
		}
		Vector& GetChildNodes();
		const Vector& GetChildNodes() const;
		size_t GetChildNodesCount() const;

		void InsertChild(KxDataViewMainWindow* window, KxDataViewTreeNode* node, size_t index);
		void RemoveChild(size_t index);

		// Should be called after changing the item value to update its position in the control if necessary.
		void PutInSortOrder(KxDataViewMainWindow* window)
		{
			if (m_ParentNode)
			{
				m_ParentNode->PutChildInSortOrder(window, this);
			}
		}
		void Resort(KxDataViewMainWindow* window);

		// Returns position of child node for given item in children list or 'wxNOT_FOUND'.
		ptrdiff_t FindChildByItem(const KxDataViewItem& item) const;
		int GetIndentLevel() const;

		bool IsExpanded() const;
		void ToggleExpanded(KxDataViewMainWindow* window);

		// "HasChildren" property corresponds to model's IsContainer(). Note that it may be true even if GetChildNodes() is empty; see below.
		bool HasChildren() const
		{
			return HasBranchData();
		}
		void SetHasChildren(bool hasChildren);

		ptrdiff_t GetSubTreeCount() const;
		void ChangeSubTreeCount(ptrdiff_t num);

		const KxDataViewItem& GetItem() const
		{
			return m_Item;
		}
		void SetItem(const KxDataViewItem& item)
		{
			m_Item = item;
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxDataViewTreeRootNode: public KxDataViewTreeNode
{
	private:
		KxDataViewMainWindow* m_MainWindow = nullptr;

	public:
		KxDataViewTreeRootNode(KxDataViewMainWindow* window, KxDataViewTreeNode* parent, const KxDataViewItem& item)
			:KxDataViewTreeNode(parent, item), m_MainWindow(window)
		{
		}

	public:
		KxDataViewMainWindow* GetMainWindow() const
		{
			return m_MainWindow;
		}
};

//////////////////////////////////////////////////////////////////////////
// Helper class to perform an operation on the tree node
class KX_API KxDataViewTreeNodeOperation
{
	public:
		// The return value control how the tree-walker traverse the tree
		enum class Result
		{
			DONE, // Done, stop traversing and return
			SKIP_SUBTREE, // Ignore the current node's subtree and continue
			CONTINUE, // Not done, continue
		};

	private:
		static bool DoWalk(KxDataViewTreeNode* node, KxDataViewTreeNodeOperation& func);

	protected:
		virtual Result operator()(KxDataViewTreeNode* node) = 0;

	public:
		virtual ~KxDataViewTreeNodeOperation() {}

	public:
		bool Walk(KxDataViewTreeNode* node)
		{
			return DoWalk(node, *this);
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxDataViewTreeNodeOperation_RowToTreeNode: public KxDataViewTreeNodeOperation
{
	private:
		const ptrdiff_t m_Row = -1;
		ptrdiff_t m_CurrentRow = -1;
		KxDataViewTreeNode* m_ResultNode = nullptr;

	public:
		KxDataViewTreeNodeOperation_RowToTreeNode(ptrdiff_t row , ptrdiff_t current)
			:m_Row(row), m_CurrentRow(current)
		{
		}

	public:
		virtual Result operator()(KxDataViewTreeNode* node) override;

		KxDataViewTreeNode* GetResult() const
		{
			return m_ResultNode;
		}
};

// As with RowToTreeNodeJob above, we initialize m_current to -1 because
// the first node passed to our operator() is the root node which is not
// visible on screen and so we should return 0 for its first child node and
// not for the root itself.
class KX_API KxDataViewTreeNodeOperation_ItemToRow: public KxDataViewTreeNodeOperation
{
	public:
		using TItemIterator = KxDataViewItem::Vector::reverse_iterator;

	private:
		const KxDataViewItem m_Item;
		TItemIterator m_Iterator;
		ptrdiff_t m_Current = -1; // The row corresponding to the last node seen in our operator().

	public:
		KxDataViewTreeNodeOperation_ItemToRow(const KxDataViewItem& item, TItemIterator iterator)
			:m_Item(item), m_Iterator(iterator)
		{
		}

	public:
		virtual Result operator()(KxDataViewTreeNode* node) override;

		ptrdiff_t GetResult() const
		{
			return m_Current;
		}
};
