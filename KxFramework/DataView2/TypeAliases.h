#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "KxFramework/KxQueryInterface.h"

namespace Kx::DataView2
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

namespace Kx::DataView2
{
	class TypeAliases
	{
		public:
			using View = Kx::DataView2::View;
			using MainWindow = Kx::DataView2::MainWindow;
			using HeaderCtrl = Kx::DataView2::HeaderCtrl;

			using Renderer = Kx::DataView2::Renderer;
			using RenderEngine = Kx::DataView2::RenderEngine;
			using Editor = Kx::DataView2::Editor;
			using Column = Kx::DataView2::Column;
			using Model = Kx::DataView2::Model;
			using Node = Kx::DataView2::Node;
			using INodeModel = Kx::DataView2::INodeModel;
			using Row = Kx::DataView2::Row;
			using SortOrder = Kx::DataView2::SortOrder;
			using CellState = Kx::DataView2::CellState;

		protected:
			TypeAliases() = default;
			~TypeAliases() = default;
		};
}
