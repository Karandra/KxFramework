#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "Row.h"
#include "Column.h"
#include "SortOrder.h"
#include "INodeModel.h"
#include "KxFramework/KxQueryInterface.h"

namespace Kx::DataView2
{
	class KX_API CellAttributes;
	class KX_API MainWindow;
	class KX_API Renderer;
	class KX_API Editor;
	class KX_API View;
	class KX_API Node;
	class CellState;

	class KX_API NodeOperation_RowToNode;
	class KX_API NodeOperation_NodeToRow;
}

namespace Kx::DataView2
{
	class KX_API Node: public Kx::RTTI::IExtendInterface<Node, INodeModel>
	{
		friend class MainWindow;
		friend class NodeOperation_RowToNode;
		friend class NodeOperation_NodeToRow;

		public:
			using Vector = std::vector<Node*>;

		private:
			static Node* CreateRootNode(MainWindow* window);

		private:
			Vector m_Children;
			MainWindow* m_MainWindow = nullptr;
			Node* m_ParentNode = nullptr;

			// Total count of expanded (i.e. visible with the help of some scrolling) items
			// in the subtree, but excluding this node. I.e. it is 0 for leaves and is the
			// number of rows the subtree occupies for branch nodes.
			intptr_t m_SubTreeCount = 0;

			Row m_IndexWithinParent;

			SortOrder m_SortOrder = SortOrder::UseNone();
			bool m_IsExpanded = false;

		private:
			// Called by the child after it has been updated to put it in the right place among its siblings, depending on the sort order.
			void PutChildInSortOrder(Node* childNode);
			void Resort();

			// Should be called after changing the item value to update its position in the control if necessary.
			void PutInSortOrder()
			{
				if (m_ParentNode)
				{
					m_ParentNode->PutChildInSortOrder(this);
				}
			}
			const SortOrder& GetSortOrder() const
			{
				return m_SortOrder;
			}
			void SetSortOrder(const SortOrder& sortOrder)
			{
				m_SortOrder = sortOrder;
			}
			void ResetSortOrder()
			{
				m_SortOrder = SortOrder::UseNone();
			}

			void CalcSubTreeCount();
			intptr_t GetSubTreeCount() const;
			void ChangeSubTreeCount(intptr_t num);
			void RecalcIndexes(size_t startAt = 0);
			void InitNodeFromThis(Node& node);

			bool IsNodeExpanded() const
			{
				return m_IsExpanded;
			}
			void SetNodeExpanded(bool expanded)
			{
				CalcSubTreeCount();
				m_IsExpanded = expanded;
			}
			void ToggleNodeExpanded()
			{
				SetNodeExpanded(!IsNodeExpanded());
			}

		public:
			Node(MainWindow* window, Node* parent)
				:m_MainWindow(window), m_ParentNode(parent)
			{
			}
			Node() = default;
			virtual ~Node();

		public:
			bool IsRootNode() const
			{
				return m_ParentNode == nullptr;
			}
			bool HasParent() const
			{
				return m_ParentNode != nullptr;
			}
			Node* GetParent() const
			{
				return m_ParentNode;
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

			Row FindChild(const Node& node) const;
			Row GetIndexWithinParent() const
			{
				return m_IndexWithinParent;
			}
			int GetIndentLevel() const;

			Node* DetachChild(size_t index);
			Node* DetachChild(Node& node);
			Node* Detach();
			
			void RemoveChild(size_t index);
			void RemoveChild(Node& node);
			void Remove();

			void AttachChild(Node* node, size_t index);
			void InsertChild(Node* node, size_t index);
			void AppendChild(Node* node)
			{
				InsertChild(node, m_Children.size());
			}
			
			void MoveAt(Node& node, size_t index)
			{
				node.AttachChild(Detach(), index);
			}
			void Move(Node& node)
			{
				node.AttachChild(Detach(), node.GetChildrenCount());
			}
			bool Swap(Node& otherNode);

			template<class TNode, class... Args> TNode& NewChild(Args&&... arg)
			{
				TNode* node = new TNode(std::forward<Args>(arg)...);
				InsertChild(node, m_Children.size());
				return *node;
			}
			template<class TNode, class... Args> TNode& NewChildAt(size_t index, Args&&... arg)
			{
				TNode* node = new TNode(std::forward<Args>(arg)...);
				InsertChild(node, index);
				return *node;
			}
			
		public:
			MainWindow* GetMainWindow() const;
			View* GetView() const;
			bool IsRenderable(const Column& column) const;

			bool IsExpanded() const;
			void SetExpanded(bool expanded);
			void Expand();
			void Collapse();
			void ToggleExpanded();

			void Refresh();
			void Refresh(Column& column);
			void Edit(Column& column);

			Row GetRow() const;
			bool IsSelected() const;
			bool IsCurrent() const;
			bool IsHotTracked() const;
			void SetSelected(bool value);
			void Select()
			{
				SetSelected(true);
			}
			void Unselect()
			{
				SetSelected(false);
			}
			void EnsureVisible(const Column* column = nullptr);

			wxRect GetCellRect(const Column* column = nullptr) const;
			wxPoint GetDropdownMenuPosition(const Column* column = nullptr) const;
		};
}

namespace Kx::DataView2
{
	class KX_API NodeOperation
	{
		public:
			// The return value control how the tree-walker traverse the tree
			enum class Result
			{
				Done, // Done, stop traversing and return
				SkipSubTree, // Ignore the current node's subtree and continue
				Continue, // Not done, continue
			};

		private:
			static bool DoWalk(Node& node, NodeOperation& func);

		protected:
			virtual Result operator()(Node& node) = 0;

		public:
			virtual ~NodeOperation() = default;

		public:
			bool Walk(Node& node)
			{
				return DoWalk(node, *this);
			}
	};
}

namespace Kx::DataView2
{
	class KX_API NodeOperation_RowToNode: public NodeOperation
	{
		private:
			const intptr_t m_Row = -1;
			intptr_t m_CurrentRow = -1;
			Node* m_ResultNode = nullptr;

		public:
			NodeOperation_RowToNode(intptr_t row , intptr_t current)
				:m_Row(row), m_CurrentRow(current)
			{
			}

		public:
			Result operator()(Node& node) override;

			Node* GetResult() const
			{
				return m_ResultNode;
			}
	};
}

namespace Kx::DataView2
{
	// As with 'NodeOperation_RowToNode' above, we initialize 'm_CurrentRow' to -1 because the first node
	// passed to our operator() is the root node which is not visible on screen and so we
	// should return 0 for its first child node and not for the root itself.
	class KX_API NodeOperation_NodeToRow: public NodeOperation
	{
		public:
			using TNodeIterator = Node::Vector::reverse_iterator;

		private:
			const Node& m_Node;
			TNodeIterator m_Iterator;
			intptr_t m_CurrentRow = -1; // The row corresponding to the last node seen in our operator().

		public:
			NodeOperation_NodeToRow(const Node& node, TNodeIterator iterator)
				:m_Node(node), m_Iterator(iterator)
			{
			}

		public:
			Result operator()(Node& node) override;

			Row GetResult() const
			{
				return m_CurrentRow >= 0 ? Row(m_CurrentRow) : Row();
			}
	};
}
