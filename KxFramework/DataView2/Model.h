#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "Node.h"
#include "Row.h"

namespace KxDataView2
{
	class KX_API View;
	class KX_API Node;
	class KX_API Column;
	class KX_API MainWindow;
	class KX_API CellAttributes;
}

namespace KxDataView2
{
	class KX_API Model
	{
		friend class MainWindow;

		private:
			MainWindow* m_MainWindow = nullptr;
			const bool m_OwnTreeItems = false;

		private:
			void SetMainWindow(MainWindow* mainWindow)
			{
				m_MainWindow = mainWindow;
			}

		public:
			Model(bool ownTreeItems)
				:m_OwnTreeItems(ownTreeItems)
			{
			}
			virtual ~Model() = default;

		public:
			MainWindow* GetMainWindow() const
			{
				return m_MainWindow;
			}
			bool OwnTreeItems() const
			{
				return m_OwnTreeItems;
			}

			void CellChanged(Node& node, Column& column)
			{
				node.Refresh(column);
			}
	};
}

//////////////////////////////////////////////////////////////////////////
namespace KxDataView2
{
	class KX_API ListModel: public Model
	{
		public:
			virtual size_t GetItemCount() const = 0;

			virtual Row GetRow(const Node& node) const;
			virtual Node* GetNode(Row row) const;

		public:
			ListModel(bool ownTreeItems)
				:Model(ownTreeItems)
			{
			}

		public:
			void RowCellChanged(Row row, Column& column)
			{
				if (Node* node = GetNode(row))
				{
					CellChanged(*node, column);
				}
			}
	};
}

namespace KxDataView2
{
	class KX_API VirtualListModel: public ListModel
	{
		private:
			const size_t m_InitialCount = 0;
			size_t m_ItemsCount = 0;

		private:
			void OnRowInserted(Row row);
			void OnRowRemoved(Row row);

		public:
			VirtualListModel(size_t initialCount = 0)
				:ListModel(true), m_InitialCount(initialCount)
			{
			}

		public:
			size_t GetInitialCount() const
			{
				return m_InitialCount;
			}
			size_t GetItemCount() const override
			{
				return m_ItemsCount;
			}
			void SetItemCount(size_t newCount)
			{
				m_ItemsCount = newCount;
			}

			void RowPrepended()
			{
				m_ItemsCount++;
				OnRowInserted(0);
			}
			void RowInserted(Row row)
			{
				m_ItemsCount++;
				OnRowInserted(row);
			}
			void RowAppended()
			{
				m_ItemsCount++;
				OnRowInserted(m_ItemsCount);
			}
			void RowRemoved(Row row) 
			{
				m_ItemsCount--;
				OnRowRemoved(row);
			}
	};
}
