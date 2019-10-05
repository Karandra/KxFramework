#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "ToolTip.h"
#include "Node.h"
#include "Row.h"
#include <Kx/RTTI.hpp>

namespace KxDataView2
{
	class KX_API View;
	class KX_API Column;
	class KX_API MainWindow;
	class KX_API CellAttributes;
}

namespace KxDataView2
{
	class KX_API Model: public KxRTTI::Interface<Model>
	{
		friend class KX_API Node;
		friend class KX_API RootNode;
		friend class KX_API MainWindow;

		private:
			MainWindow* m_MainWindow = nullptr;

		private:
			void SetMainWindow(MainWindow* mainWindow)
			{
				m_MainWindow = mainWindow;
			}

		protected:
			virtual void OnAttachModel()
			{
			}
			virtual void OnDetachModel()
			{
			}

		public:
			virtual ~Model() = default;

		public:
			MainWindow* GetMainWindow() const
			{
				return m_MainWindow;
			}
			Node& GetRootNode() const;
			View* GetView() const;

			void ItemsChanged();
			void CellChanged(Node& node, Column& column)
			{
				node.Refresh(column);
			}
			
			bool IsEditable(Node& node, const Column& column) const;
			bool IsActivatable(Node& node, const Column& column) const;

			virtual Renderer& GetRenderer(const Node& node, const Column& column) const;
			virtual Editor* GetEditor(const Node& node, const Column& column) const;
			virtual bool IsEnabled(const Node& node, const Column& column) const
			{
				return true;
			}

			virtual wxAny GetValue(const Node& node, const Column& column) const
			{
				return {};
			}
			virtual wxAny GetEditorValue(const Node& node, const Column& column) const
			{
				return {};
			}
			virtual ToolTip GetToolTip(const Node& node, const Column& column) const;
			virtual bool SetValue(Node& node, Column& column, const wxAny& value)
			{
				return false;
			}

			virtual bool GetAttributes(const Node& node, const Column& column, const CellState& cellState, CellAttributes& attributes) const
			{
				return false;
			}
			virtual bool IsCategoryNode(const Node& node) const
			{
				return false;
			}
			virtual int GetRowHeight(const Node& node) const
			{
				return 0;
			}

			virtual bool Compare(const Node& leftNode, const Node& rightNode, const Column& column) const
			{
				return false;
			}
	};
}

//////////////////////////////////////////////////////////////////////////
namespace KxDataView2
{
	class KX_API ListModel: public KxRTTI::ExtendInterface<ListModel, Model>
	{
		public:
			size_t GetItemCount() const;

			Row GetRow(const Node& node) const;
			Node* GetNode(Row row) const;

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
	class KX_API VirtualListModel: public KxRTTI::ExtendInterface<VirtualListModel, Model>
	{
		private:
			size_t m_ItemCount = 0;

		private:
			VirtualNode& GetVirtualNode() const;

			void OnRowInserted(Row row);
			void OnRowRemoved(Row row);

		public:
			size_t GetItemCount() const
			{
				return m_ItemCount;
			}
			void SetItemCount(size_t count)
			{
				m_ItemCount = count;
				ItemsChanged();
			}

			void RowPrepended()
			{
				m_ItemCount++;
				OnRowInserted(0);
			}
			void RowInserted(Row row)
			{
				m_ItemCount++;
				OnRowInserted(row);
			}
			void RowAppended()
			{
				m_ItemCount++;
				OnRowInserted(m_ItemCount);
			}
			void RowRemoved(Row row) 
			{
				m_ItemCount--;
				OnRowRemoved(row);
			}
	};
}
