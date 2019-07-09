#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "Row.h"
#include "Column.h"
#include "SortOrder.h"
#include "ToolTip.h"
#include "KxFramework/KxQueryInterface.h"
#include "KxFramework/KxFunctional.h"

namespace KxDataView2
{
	class KX_API CellAttributes;
	class KX_API MainWindow;
	class KX_API Renderer;
	class KX_API Editor;
	class KX_API Model;
	class KX_API View;
	class KX_API Node;
	class CellState;

	class KX_API NodeOperation_RowToNode;
}

namespace KxDataView2
{
	class KX_API RootNode;
	class KX_API VirtualNode;

	class KX_API Node: public KxRTTI::IInterface<Node>
	{
		friend class RootNode;
		friend class VirtualNode;
		friend class MainWindow;
		friend class NodeOperation_RowToNode;

		public:
			using Vector = std::vector<Node*>;

		private:
			Vector m_Children;
			Node* m_ParentNode = nullptr;
			RootNode* m_RootNode = nullptr;

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
			void Resort(bool force = false);

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

			intptr_t GetSubTreeCount() const
			{
				return m_SubTreeCount;
			}
			void ChangeSubTreeCount(intptr_t num);
			void RecalcIndexes(size_t startAt = 0);
			void InitNodeUsing(const Node& node);

			bool IsNodeExpanded() const
			{
				return m_IsExpanded;
			}
			void SetNodeExpanded(bool expanded)
			{
				m_IsExpanded = expanded;
			}
			intptr_t ToggleNodeExpanded();

			void ResetAll()
			{
				*this = Node();
			}

		public:
			Node() = default;
			~Node();

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

			void DetachAllChildren();
			Node* DetachChild(size_t index);
			Node* DetachChild(Node& node);
			Node* DetachThis()
			{
				if (m_ParentNode)
				{
					return m_ParentNode->DetachChild(*this);
				}
				return nullptr;
			}

			void AttachChild(Node& node, size_t index);
			void AttachChild(Node& node)
			{
				AttachChild(node, GetChildrenCount());
			}
			void AttachChildren(std::initializer_list<std::reference_wrapper<Node>> list)
			{
				for (Node& node: list)
				{
					AttachChild(node, GetChildrenCount());
				}
			}
			template<class TContainer> void AttachChildren(TContainer& items)
			{
				auto GetRawPointer = [](auto& node)
				{
					using TValue = std::decay_t<decltype(node)>;
					if constexpr(KxFunctional::is_unique_ptr_v<TValue>)
					{
						return node.get();
					}
					else if constexpr(std::is_pointer_v<TValue>)
					{
						return node;
					}
					else
					{
						return &node;
					}
				};

				for (auto& node: items)
				{
					AttachChild(*GetRawPointer(node), GetChildrenCount());
				}
			}
			
			bool MoveTo(Node& node, size_t index)
			{
				if (Node* thisNode = DetachThis())
				{
					node.AttachChild(*thisNode, index);
					return true;
				}
				return false;
			}
			bool MoveTo(Node& node)
			{
				return MoveTo(node, node.GetChildrenCount());
			}
			bool Swap(Node& otherNode);

		public:
			MainWindow* GetMainWindow() const;
			View* GetView() const;
			Model* GetModel() const;
			bool IsRenderable(const Column& column) const;

			bool IsExpanded() const;
			void SetExpanded(bool expand);
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
			wxRect GetClientCellRect(const Column* column = nullptr) const;
			wxPoint GetDropdownMenuPosition(const Column* column = nullptr) const;

		public:
			bool IsEditable(const Column& column) const;
			bool IsActivatable(const Column& column) const;

			virtual Renderer& GetRenderer(const Column& column) const;
			virtual Editor* GetEditor(const Column& column) const;
			virtual bool IsEnabled(const Column& column) const;

			virtual wxAny GetValue(const Column& column) const;
			virtual wxAny GetEditorValue(const Column& column) const;
			virtual ToolTip GetToolTip(const Column& column) const;
			virtual bool SetValue(Column& column, const wxAny& value);

			virtual bool GetAttributes(const Column& column, const CellState& cellState, CellAttributes& attributes) const;
			virtual bool IsCategoryNode() const;
			virtual int GetRowHeight() const;

			virtual bool Compare(const Node& other, const Column& column) const;
		};
}

namespace KxDataView2
{
	class KX_API RootNode: public KxRTTI::IExtendInterface<RootNode, Node>
	{
		friend class MainWindow;

		private:
			MainWindow* const m_MainWindow = nullptr;

		private:
			void Init()
			{
				m_RootNode = this;
				SetNodeExpanded(true);
			}
			void ResetAll();

		public:
			RootNode(MainWindow* mainWindow)
				:m_MainWindow(mainWindow)
			{
				Init();
			}

		public:
			MainWindow* GetMainWindow() const
			{
				return m_MainWindow;
			}
			View* GetView() const;
			Model* GetModel() const;
	};
}

namespace KxDataView2
{
	class KX_API VirtualNode: public KxRTTI::IExtendInterface<VirtualNode, Node>
	{
		friend class MainWindow;
		friend class VirtualListModel;

		protected:
			class VirtualRowChanger
			{
				private:
					VirtualNode& m_Node;
					const Row m_OriginalRow;

				public:
					VirtualRowChanger(VirtualNode& node, Row row)
						:m_Node(node), m_OriginalRow(node.GetVirtualRow())
					{
						m_Node.SetVirtualRow(row);
					}
					~VirtualRowChanger()
					{
						m_Node.SetVirtualRow(m_OriginalRow);
					}
			
				public:
					VirtualNode& GetNode()
					{
						return m_Node;
					}
			};

		protected:
			Row GetVirtualRow() const
			{
				return m_IndexWithinParent;
			}
			void SetVirtualRow(Row row)
			{
				m_IndexWithinParent = row;
			}

		public:
			VirtualNode(RootNode& rootNode, Row row = {})
			{
				InitNodeUsing(rootNode);
				SetVirtualRow(row);
			}
	};
}

namespace KxDataView2
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

namespace KxDataView2
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
