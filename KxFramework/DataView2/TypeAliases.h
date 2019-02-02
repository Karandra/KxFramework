#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "KxFramework/KxQueryInterface.h"

namespace KxDataView2
{
	class KX_API View;
	class KX_API MainWindow;
	class KX_API HeaderCtrl;

	class KX_API CellAttributes;
	class KX_API Renderer;
	class KX_API RenderEngine;
	class KX_API Editor;
	class KX_API Column;
	class KX_API Node;
	class KX_API Model;
	class KX_API INodeModel;
	class Row;
	class SortOrder;
	class CellState;
}

namespace KxDataView2
{
	class TypeAliases
	{
		public:
			using View = KxDataView2::View;
			using MainWindow = KxDataView2::MainWindow;
			using HeaderCtrl = KxDataView2::HeaderCtrl;

			using Renderer = KxDataView2::Renderer;
			using RenderEngine = KxDataView2::RenderEngine;
			using Editor = KxDataView2::Editor;
			using Column = KxDataView2::Column;
			using Model = KxDataView2::Model;
			using Node = KxDataView2::Node;
			using INodeModel = KxDataView2::INodeModel;
			using Row = KxDataView2::Row;
			using SortOrder = KxDataView2::SortOrder;
			using CellState = KxDataView2::CellState;
			using CellAttributes = KxDataView2::CellAttributes;

		protected:
			TypeAliases() = default;
			~TypeAliases() = default;
		};
}
