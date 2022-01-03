#include "KxfPCH.h"
#include "Node.h"
#include "SortMode.h"
#include "CellAttributes.h"
#include "../Widgets/WXUI/DataView/View.h"
#include "../Widgets/WXUI/DataView/MainWindow.h"
#include "../Widgets/WXUI/DataView/HeaderCtrl.h"

namespace kxf::DataView
{
	size_t Node::CalcSubTreeIndex() const
	{
		if (m_ParentNode)
		{
			size_t index = 0;
			for (const Node& node: m_ParentNode->m_Children)
			{
				if (node.IsSameAs(*this))
				{
					return index;
				}
				else
				{
					index++;
				}
			}
		}
		return npos;
	}
	size_t Node::CalcSubTreeCount() const
	{
		// Total count of expanded (i.e. visible with the help of some scrolling) items
		// in the subtree, but excluding this node. I.e. it is 0 for leaves and is the
		// number of rows the subtree occupies for branch nodes.

		size_t count = 0;
		if (m_IsExpanded || IsRootNode())
		{
			for (const Node& node: m_Children)
			{
				count += node.GetSubTreeCount() + 1;
			}
		}
		return count;
	}
	void Node::ChangeSubTreeCount(intptr_t num)
	{
		if (m_IsExpanded)
		{
			if (m_SubTreeCount != npos)
			{
				m_SubTreeCount += num;
				if (m_ParentNode)
				{
					m_ParentNode->ChangeSubTreeCount(num);
				}
			}
			else
			{
				RecalcSubTreeCount();
				if (m_ParentNode)
				{
					m_ParentNode->RecalcSubTreeCount();
				}
			}
		}
	}

	void Node::RefreshChildren()
	{
		auto DoEnumerate = [&](auto& ref)
		{
			for (auto& node: m_Children)
			{
				node.OnDetach();
			}
			m_Children.clear();

			const size_t count = ref.GetChildrenCount();
			m_Children.reserve(count);
			for (size_t i = 0; i < count; i++)
			{
				m_Children.emplace_back(ref.GetChildItem(i), m_ParentNode);
			}

			for (auto& node: m_Children)
			{
				node.OnAttach();
			}
		};

		if (IsRootNode())
		{
			DoEnumerate(GetDataModel());
		}
		else if (m_Item)
		{
			DoEnumerate(*m_Item);
		}
		m_SubTreeCount = npos;
	}
	void Node::SortChildren()
	{
		auto sortMode = GetView().GetSortMode();
		OnSortChildren(sortMode);

		std::sort(m_Children.begin(), m_Children.end(), [&](const Node& left, const Node& right)
		{
			if (left.m_Item && right.m_Item)
			{
				return left.m_Item->Compare(*right.m_Item, sortMode);
			}
			return std::partial_ordering::unordered;
		});
	}

	size_t Node::ToggleNodeExpandState()
	{
		// We do not allow the (invisible) root node to be collapsed because there is no way to expand it again.
		if (!IsRootNode())
		{
			intptr_t count = 0;
			for (const Node& node: m_Children)
			{
				count += static_cast<intptr_t>(node.GetSubTreeCount()) + 1;
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
				SortChildren();
			}
		}
		return m_SubTreeCount;
	}
	void Node::DoExpandNodeAncestors()
	{
		Node* node = m_ParentNode;
		while (node)
		{
			node->ExpandNode();
			node = node->m_ParentNode;
		}
	}
	void Node::DoEnsureCellVisible(const Column* column)
	{
		DoExpandNodeAncestors();

		if (Row row = GetRow())
		{
			auto& mainWindow = GetMainWindow();
			mainWindow.RecalculateDisplay();

			if (column)
			{
				mainWindow.EnsureVisible(row, column->GetIndex());
			}
			else
			{
				mainWindow.EnsureVisible(row);
			}
		}
	}

	Rect Node::DoGetCellRect(const Column* column) const
	{
		Rect rect = DoGetCellClientRect(column);
		if (auto header = GetView().GetHeaderCtrl())
		{
			rect.SetTop(rect.GetTop() + header->GetSize().GetHeight());
		}
		return rect;
	}
	Rect Node::DoGetCellClientRect(const Column* column) const
	{
		return GetMainWindow().GetItemRect(*this, column);
	}
	Point Node::DoGetCellDropdownPoint(const Column* column) const
	{
		return DoGetCellRect(column).GetLeftBottom() + Point(GetMainWindow().FromDIP(Point(0, 1)));
	}

	WXUI::DataView::View& Node::GetView() const
	{
		return *m_RootNode->m_View;
	}
	WXUI::DataView::MainWindow& Node::GetMainWindow() const
	{
		return *m_RootNode->m_MainWindow;
	}
	IDataViewModel& Node::GetDataModel() const
	{
		return *m_RootNode->m_DataModel;
	}

	bool Node::IsAttached() const noexcept
	{
		return m_RootNode && m_RootNode->m_DataModel;
	}
	void Node::ExpandNode()
	{
		DoExpandNodeAncestors();
		GetMainWindow().Expand(*this);
	}
	void Node::CollapseNode()
	{
		GetMainWindow().Collapse(*this);
	}

	void Node::RefreshCell()
	{
		GetMainWindow().OnCellChanged(*this, nullptr);
	}
	void Node::RefreshCell(Column& column)
	{
		GetMainWindow().OnCellChanged(*this, &column);
	}
	bool Node::EditCell(Column& column)
	{
		return GetMainWindow().BeginEdit(*this, column);
	}

	Row Node::GetRow() const
	{
		return GetMainWindow().GetRowByNode(*this);
	}
	int Node::GetIndentLevel() const
	{
		if (IsRootNode())
		{
			return -1;
		}
		else
		{
			int level = 0;

			const Node* node = this;
			while (node->m_ParentNode && node->m_ParentNode->m_ParentNode)
			{
				node = node->m_ParentNode;
				level++;
			}
			return level;
		}
	}

	CellState Node::GetCellState() const
	{
		return GetMainWindow().GetCellStateForRow(GetRow());
	}
	void Node::SelectItem()
	{
		DoExpandNodeAncestors();
		if (Row row = GetRow())
		{
			auto& mainWindow = GetMainWindow();

			// Unselect all rows before select another in the single select mode
			if (mainWindow.IsSingleSelection())
			{
				mainWindow.UnselectAllRows();
			}

			// Select the row and set focus to the selected item
			mainWindow.SelectRow(row);
			mainWindow.ChangeCurrentRow(row);
		}
	}
	void Node::UnselectItem()
	{
		GetMainWindow().SelectRow(GetRow(), false);
	}
	void Node::MakeCurrent()
	{
		auto& mainWindow = GetMainWindow();
		if (mainWindow.IsMultipleSelection())
		{
			Row newCurrent = GetRow();
			Row oldCurrent = mainWindow.GetCurrentRow();

			if (newCurrent != oldCurrent)
			{
				mainWindow.ChangeCurrentRow(newCurrent);
				mainWindow.RefreshRow(oldCurrent);
				mainWindow.RefreshRow(newCurrent);
			}
		}
		else
		{
			SelectItem();
		}
	}
}
