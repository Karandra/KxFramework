#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxQueryInterface.h"
#include "DataView2Fwd.h"

namespace KxDataView2
{
	class TypeAliases
	{
		public:
			using View = KxDataView2::View;
			using MainWindow = KxDataView2::MainWindow;
			using HeaderCtrl = KxDataView2::HeaderCtrl;
			using Event = KxDataView2::Event;

			using Renderer = KxDataView2::Renderer;
			using RenderEngine = KxDataView2::RenderEngine;
			using Editor = KxDataView2::Editor;
			using EditorControlHandler = KxDataView2::EditorControlHandler;
			using Column = KxDataView2::Column;
			using Node = KxDataView2::Node;
			using INodeModel = KxDataView2::INodeModel;
			
			using Model = KxDataView2::Model;
			using ListModel = KxDataView2::ListModel;
			using VirtualListModel = KxDataView2::VirtualListModel;

			using Row = KxDataView2::Row;
			using ColumnID = KxDataView2::ColumnID;
			using CellState = KxDataView2::CellState;
			using CellAttributes = KxDataView2::CellAttributes;
			using SortOrder = KxDataView2::SortOrder;

		protected:
			TypeAliases() = default;
			~TypeAliases() = default;
		};
}
