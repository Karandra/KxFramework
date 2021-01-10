#pragma once
#include "Common.h"
#include "ToolTip.h"
#include "Node.h"
#include "Row.h"
#include "kxf/General/Any.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf::UI::DataView
{
	class View;
	class Column;
	class MainWindow;
	class CellAttribute;
}

namespace kxf::UI::DataView
{
	class KX_API Model: public RTTI::Interface<Model>
	{
		KxRTTI_DeclareIID(Model, {0xf9971234, 0xa17b, 0x42bf, {0x84, 0x9a, 0xa9, 0x13, 0xf1, 0x20, 0xc9, 0x35}});

		friend class Node;
		friend class RootNode;
		friend class MainWindow;

		private:
			MainWindow* m_MainWindow = nullptr;
			View* m_View = nullptr;

		private:
			void DoOnModelAttached(MainWindow& mainWindow);
			void DoOnModelDetached();

		protected:
			virtual void OnModelAttached()
			{
			}
			virtual void OnModelDetached()
			{
			}

		public:
			virtual ~Model() = default;

		public:
			void NotifyItemsChanged();
			MainWindow* GetMainWindow() const
			{
				return m_MainWindow;
			}
			View* GetView() const
			{
				return m_View;
			}

			virtual RootNode& GetRootNode() const = 0;

			virtual Renderer& GetRenderer(const Node& node, const Column& column) const;
			virtual Editor* GetEditor(const Node& node, const Column& column) const;
			virtual bool IsEnabled(const Node& node, const Column& column) const
			{
				return true;
			}

			virtual Any GetValue(const Node& node, const Column& column) const
			{
				return {};
			}
			virtual Any GetEditorValue(const Node& node, const Column& column) const
			{
				return {};
			}
			virtual ToolTip GetToolTip(const Node& node, const Column& column) const;
			virtual bool SetValue(Node& node, Column& column, const Any& value)
			{
				return false;
			}

			virtual CellAttribute GetAttributes(const Node& node, const Column& column, const CellState& cellState) const
			{
				return {};
			}
			virtual int GetRowHeight(const Node& node) const
			{
				return 0;
			}
	};
}
