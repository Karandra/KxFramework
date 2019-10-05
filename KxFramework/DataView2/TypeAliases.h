#pragma once
#include "KxFramework/KxFramework.h"
#include "DataView2Fwd.h"

namespace KxDataView2
{
	struct TypeAliases
	{
		using View = KxDataView2::View;
		using MainWindow = KxDataView2::MainWindow;
		using HeaderCtrl = KxDataView2::HeaderCtrl;
		using DropSource = KxDataView2::DropSource;
		using DropTarget = KxDataView2::DropTarget;

		using Renderer = KxDataView2::Renderer;
		using RenderEngine = KxDataView2::RenderEngine;
		using Editor = KxDataView2::Editor;
		using Column = KxDataView2::Column;
		using Node = KxDataView2::Node;
			
		using Model = KxDataView2::Model;
		using ListModel = KxDataView2::ListModel;
		using VirtualListModel = KxDataView2::VirtualListModel;

		using Event = KxDataView2::Event;
		using EventDND = KxDataView2::EventDND;
		using EventEditor = KxDataView2::EventEditor;

		using Row = KxDataView2::Row;
		using ColumnID = KxDataView2::ColumnID;
		using CellState = KxDataView2::CellState;
		using CellAttributes = KxDataView2::CellAttributes;
		using SortOrder = KxDataView2::SortOrder;
		using ToolTip = KxDataView2::ToolTip;

		using CtrlStyle = KxDataView2::CtrlStyle;
		using CtrlExtraStyle = KxDataView2::CtrlExtraStyle;
		using ColumnStyle = KxDataView2::ColumnStyle;
		using DNDOpType = KxDataView2::DNDOpType;
		using ColumnWidth = KxDataView2::ColumnWidth;
		using UniformHeight = KxDataView2::UniformHeight;
	};
}
