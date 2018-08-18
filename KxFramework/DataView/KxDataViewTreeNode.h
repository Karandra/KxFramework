#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/DataView/KxDataViewItem.h"
class KxDataViewMainWindow;
class KxDataViewTreeNodeData;

class KxDataViewTreeNode
{
	public:
		using Vector = std::vector<KxDataViewTreeNode*>;

		static KxDataViewTreeNode* CreateRootNode(KxDataViewMainWindow* window);
		static bool SwapNodes(KxDataViewTreeNode* node1, KxDataViewTreeNode* node2);

	private:
		KxDataViewMainWindow* m_MainWindow = NULL;
		KxDataViewTreeNode* m_ParentNode = NULL;
		KxDataViewItem m_Item;

		// Data specific to non-leaf (branch, inner) nodes. They are kept in a separate class in order to conserve memory.
		std::unique_ptr<KxDataViewTreeNodeData> m_BranchData;

	private:
		bool HasBranchData() const
		{
			return m_BranchData != NULL;
		}
		
	public:
		KxDataViewTreeNode(KxDataViewMainWindow* window, KxDataViewTreeNode* parent, const KxDataViewItem& item);
		virtual ~KxDataViewTreeNode();

	public:
		bool IsRootNode()
		{
			return m_ParentNode == NULL;
		}
		bool HasParent() const
		{
			return m_ParentNode != NULL;
		}
		
		KxDataViewTreeNode* GetParent() const
		{
			return m_ParentNode;
		}
		Vector& GetChildNodes();
		const Vector& GetChildNodes() const;

		void InsertChild(KxDataViewTreeNode* node, size_t index);
		bool RemoveChild(KxDataViewTreeNode* node);
		void Resort(bool noRecurse = false);

		// Returns position of child node for given item in children list or 'wxNOT_FOUND'.
		ptrdiff_t FindChildByItem(const KxDataViewItem& item) const;
		int GetIndentLevel() const;

		bool IsExpanded() const;
		void ToggleExpanded();

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

class KxDataViewTreeNodeData
{
	public:
		using Vector = KxDataViewTreeNode::Vector;

	private:
		// Child nodes. Note that this may be empty in case
		// this branch of the tree wasn't expanded and realized yet.
		Vector m_Children;

		// Is the branch node currently expanded?
		bool m_IsExpanded = false;

		// Total count of expanded (i.e. visible with the help of some
		// scrolling) items in the subtree, but excluding this node. I.e. it is
		// 0 for leaves and is the number of rows the subtree occupies for
		// branch nodes.
		ptrdiff_t m_SubTreeCount = 0;

	private:
		void DeleteChildNode(KxDataViewTreeNode* node)
		{
			delete node;
		}

	public:
		KxDataViewTreeNodeData(bool isExpanded = false)
			:m_IsExpanded(isExpanded)
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

		void DeleteChild(Vector::const_iterator it)
		{
			DeleteChildNode(*it);
			m_Children.erase(it);
		}
		KxDataViewTreeNode* InsertNode(Vector::const_iterator it, KxDataViewTreeNode* node)
		{
			return *m_Children.emplace(it, node);
		}
};

//////////////////////////////////////////////////////////////////////////
// Helper class to perform an operation on the tree node
class KxDataViewTreeNodeOperation
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
class KxDataViewTreeNodeOperation_RowToTreeNode: public KxDataViewTreeNodeOperation
{
	private:
		ptrdiff_t m_Row = -1;
		ptrdiff_t m_CurrentRow = -1;
		KxDataViewTreeNode* m_ResultNode = NULL;
		KxDataViewTreeNode* m_ParentNode = NULL;

	public:
		KxDataViewTreeNodeOperation_RowToTreeNode(ptrdiff_t row , ptrdiff_t current, KxDataViewTreeNode* node)
			:m_Row(row), m_CurrentRow(current), m_ParentNode(node)
		{
		}

	public:
		virtual Result operator()(KxDataViewTreeNode* node) override;

		KxDataViewTreeNode* GetResult() const
		{
			return m_ResultNode;
		}
};

class KxDataViewTreeNodeOperation_ItemToRow: public KxDataViewTreeNodeOperation
{
	private:
		using IterT = KxDataViewItem::Vector::reverse_iterator;

		KxDataViewItem m_Item;
		IterT m_Iter;
		ptrdiff_t m_Result = -1;

	public:
		KxDataViewTreeNodeOperation_ItemToRow(const KxDataViewItem& item, IterT iterator)
			:m_Item(item), m_Iter(iterator)
		{
		}

	public:
		virtual Result operator()(KxDataViewTreeNode* node) override;

		// The row number is begin from zero
		ptrdiff_t GetResult() const
		{
			return m_Result - 1;
		}
};
