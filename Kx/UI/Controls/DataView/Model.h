#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "ToolTip.h"
#include "Node.h"
#include "Row.h"
#include <Kx/RTTI.hpp>

namespace KxFramework::UI::DataView
{
	class View;
	class Column;
	class MainWindow;
	class CellAttribute;
}

namespace KxFramework::UI::DataView
{
	class KX_API Model: public RTTI::Interface<Model>
	{
		KxDecalreIID(Model, {0xf9971234, 0xa17b, 0x42bf, {0x84, 0x9a, 0xa9, 0x13, 0xf1, 0x20, 0xc9, 0x35}});

		friend class Node;
		friend class RootNode;
		friend class MainWindow;

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

			virtual bool GetAttributes(const Node& node, const Column& column, const CellState& cellState, CellAttribute& attributes) const
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
namespace KxFramework::UI::DataView
{
	class KX_API ListModel: public RTTI::ExtendInterface<ListModel, Model>
	{
		KxDecalreIID(ListModel, {0x98e3d563, 0x5ddf, 0x458c, {0xa8, 0x8, 0x70, 0x55, 0xb0, 0x49, 0x2e, 0xb3}});

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

namespace KxFramework::UI::DataView
{
	class KX_API VirtualListModel: public RTTI::ExtendInterface<VirtualListModel, Model>
	{
		KxDecalreIID(VirtualListModel, {0x90797bbe, 0xe618, 0x4351, {0xa2, 0x86, 0x1, 0xea, 0xe8, 0x4c, 0x73, 0x48}});

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
