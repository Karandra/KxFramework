#pragma once
#include "Common.h"
#include "Row.h"
#include "CellState.h"
#include "../IDataViewItem.h"

namespace kxf::WXUI::DataView
{
	class View;
	class MainWindow;
}
namespace kxf::DataView
{
	class RootNode;
}

namespace kxf::DataView
{
	class KX_API Node
	{
		friend class RootNode;
		friend class NodeOperation;
		friend class RowToNodeOperation;
		friend class WXUI::DataView::View;
		friend class WXUI::DataView::MainWindow;

		public:
			static constexpr auto npos = std::numeric_limits<size_t>::max();

		private:
			std::shared_ptr<IDataViewItem> m_Item;
			RootNode* m_RootNode = nullptr;
			Node* m_ParentNode = nullptr;
			std::vector<Node> m_Children;

			mutable size_t m_SubTreeCount = npos;
			bool m_IsExpanded = false;

		private:
			size_t CalcSubTreeIndex() const;
			size_t CalcSubTreeCount() const;
			void RecalcSubTreeCount()
			{
				m_SubTreeCount = GetSubTreeCount();
			}
			void ChangeSubTreeCount(intptr_t num);

			void RefreshChildren();
			void SortChildren();
			void OnSortChildren(const DataView::SortMode& sortMode)
			{
				if (IsRootNode())
				{
					GetDataModel().OnSortChildren(sortMode);
				}
				else if (m_Item)
				{
					m_Item->OnSortChildren(sortMode);
				}
			}

			size_t ToggleNodeExpandState();
			void DoExpandNodeAncestors();
			void DoEnsureCellVisible(const Column* column);
			Rect DoGetCellRect(const Column* column) const;
			Rect DoGetCellClientRect(const Column* column) const;
			Point DoGetCellDropdownPoint(const Column* column) const;

			WXUI::DataView::View& GetView() const;
			WXUI::DataView::MainWindow& GetMainWindow() const;
			IDataViewModel& GetDataModel() const;

			void OnAttach()
			{
				m_Item->OnAttach(*this);
			}
			void OnDetach()
			{
				m_Item->OnDetach(*this);
				m_Item = nullptr;
			}

		protected:
			bool OnNodeExpand()
			{
				return m_Item ? m_Item->OnExpand(*this) : true;
			}
			bool OnNodeCollapse()
			{
				return m_Item ? m_Item->OnCollapse(*this) : true;
			}

			bool IsNull() const noexcept
			{
				if (IsRootNode())
				{
					return !IsAttached();
				}
				else
				{
					return !m_Item || !IsAttached();
				}
			}
			void Initialize(Node& parent)
			{
				m_ParentNode = &parent;
				m_RootNode = &parent.GetRootNode();
			}

		public:
			Node() = default;
			Node(const Node&) = delete;
			Node(std::shared_ptr<IDataViewItem> item, Node* parent)
				:m_Item(std::move(item))
			{
				if (parent)
				{
					Initialize(*parent);
				}
			}
			~Node()
			{
				if (m_Item)
				{
					OnDetach();
				}
			}

		public:
			template<std::derived_from<IObject> T = IDataViewItem>
			std::shared_ptr<T> GetItem() const noexcept
			{
				if constexpr(std::is_same_v<T, IDataViewItem>)
				{
					return m_Item;
				}
				else if (m_Item)
				{
					return m_Item->QueryInterface<T>();
				}
				return nullptr;
			}

			bool IsSameAs(const Node& other) const
			{
				return this == &other || m_Item == other.m_Item || ((m_Item && other.m_Item) && *m_Item == *other.m_Item);
			}
			bool IsRootNode() const noexcept
			{
				return this == m_RootNode && m_ParentNode == nullptr;
			}
			bool IsAttached() const noexcept;

			RootNode& GetRootNode() const
			{
				return *m_RootNode;
			}
			Node* GetParentNode() const
			{
				return m_ParentNode;
			}

			bool HasChildren() const
			{
				return m_Item ? m_Item->GetChildrenCount() != 0 : false;
			}
			size_t GetChildrenCount() const
			{
				if (IsRootNode())
				{
					return GetDataModel().GetChildrenCount();
				}
				else if (m_Item)
				{
					return m_Item->GetChildrenCount();
				}
				return 0;
			}

			void ItemChanged()
			{
				if (m_Children.size() != GetChildrenCount())
				{
					RefreshChildren();
				}
				RefreshCell();
			}
			void ChildrenChanged()
			{
				RefreshChildren();
			}

			size_t GetSubTreeCount() const
			{
				if (m_SubTreeCount == npos)
				{
					m_SubTreeCount = CalcSubTreeCount();
				}
				return m_SubTreeCount;
			}
			size_t GetSubTreeIndex() const
			{
				return CalcSubTreeIndex();
			}

			bool IsExpanded() const
			{
				return m_IsExpanded;
			}
			void ExpandNode();
			void CollapseNode();

		public:
			void RefreshCell();
			void RefreshCell(const Column& column);
			bool EditCell(Column& column);

			Row GetRow() const;
			int GetIndentLevel() const;

			CellState GetCellState() const;
			void SelectItem();
			void UnselectItem();
			void MakeCurrent();

			void EnsureCellVisible()
			{
				DoEnsureCellVisible(nullptr);
			}
			void EnsureCellVisible(const Column& column)
			{
				DoEnsureCellVisible(&column);
			}

			Rect GetCellRect() const
			{
				return DoGetCellRect(nullptr);
			}
			Rect GetCellRect(const Column& column) const
			{
				return DoGetCellRect(&column);
			}

			Rect GetCellClientRect() const
			{
				return DoGetCellClientRect(nullptr);
			}
			Rect GetCellClientRect(const Column& column) const
			{
				return DoGetCellClientRect(&column);
			}

			Point GetCellDropdownPoint() const
			{
				return DoGetCellDropdownPoint(nullptr);
			}
			Point GetCellDropdownPoint(const Column& column) const
			{
				return DoGetCellDropdownPoint(&column);
			}

		public:
			bool IsCellEditable(const Column& column) const
			{
				return IsCellEditable(column, GetCellState());
			}
			bool IsCellEditable(const Column& column, const CellState& cellState) const
			{
				return GetCellAttributes(column, cellState).Options().ContainsOption(CellStyle::Editable|CellStyle::Enabled) && GetCellEditor(column);
			}
			bool IsCellActivatable(const Column& column) const
			{
				if (auto renderer = GetCellRenderer(column))
				{
					return renderer->IsActivatable();
				}
				return false;
			}

			std::shared_ptr<DataView::CellRenderer> GetCellRenderer(const Column& column) const
			{
				return GetDataModel().GetCellRenderer(*this, column);
			}
			std::shared_ptr<DataView::CellEditor> GetCellEditor(const Column& column) const
			{
				return GetDataModel().GetCellEditor(*this, column);
			}

			Any GetCellValue(const Column& column) const
			{
				return GetDataModel().GetCellValue(*this, column);
			}
			Any GetCellDisplayValue(const Column& column) const
			{
				return GetDataModel().GetCellDisplayValue(*this, column);
			}
			bool SetCellValue(Column& column, Any value)
			{
				return GetDataModel().SetCellValue(*this, column, std::move(value));
			}

			ToolTip GetCellToolTip(const Column& column) const
			{
				return GetDataModel().GetCellToolTip(*this, column);
			}
			DataView::CellAttributes GetCellAttributes(const Column& column, const CellState& cellState) const
			{
				return GetDataModel().GetCellAttributes(*this, column, cellState);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			Node& operator=(const Node&) = delete;
	};

	class KX_API RootNode final: public Node
	{
		KxRTTI_DeclareIID(RootNode, {0xe57d0d6b, 0xdea1, 0x43d9, {0xb3, 0xf2, 0x75, 0xd1, 0xce, 0xc2, 0x32, 0x59}});
		
		friend class Node;
		friend class WXUI::DataView::View;
		friend class WXUI::DataView::MainWindow;

		private:
			IDataViewModel* m_DataModel = nullptr;
			WXUI::DataView::View* m_View = nullptr;
			WXUI::DataView::MainWindow* m_MainWindow = nullptr;

		private:
			void Initalize(WXUI::DataView::MainWindow& mainWindow) noexcept;

		public:
			RootNode() noexcept
			{
				m_RootNode = this;
				m_ParentNode = nullptr;
				m_IsExpanded = true;
			}
			RootNode(WXUI::DataView::MainWindow& mainWindow) noexcept
			{
				Initalize(mainWindow);
			}
	};
}

namespace kxf::DataView
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

namespace kxf::DataView
{
	class KX_API RowToNodeOperation final: public NodeOperation
	{
		private:
			const intptr_t m_Row = -1;
			intptr_t m_CurrentRow = -1;
			Node* m_ResultNode = nullptr;

		public:
			RowToNodeOperation(intptr_t row , intptr_t current)
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
