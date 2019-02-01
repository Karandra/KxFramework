#pragma once
#include "KxFramework/KxFramework.h"
#include "Common.h"
#include "Column.h"
#include "KxFramework/KxQueryInterface.h"

namespace Kx::DataView2
{
	class KX_API CellAttributes;
	class KX_API Renderer;
	class KX_API Editor;
	class CellState;
}

namespace Kx::DataView2
{
	class KX_API INodeModel: public Kx::RTTI::IInterface<INodeModel>
	{
		protected:
			INodeModel() = default;

		public:
			bool IsEditable(const Column& column) const;
			bool IsActivatable(const Column& column) const;

		public:
			virtual Renderer& GetRenderer(const Column& column) const
			{
				return column.GetRenderer();
			}
			virtual Editor* GetEditor(const Column& column) const
			{
				return column.GetEditor();
			}
			
			virtual wxAny GetValue(const Column& column) const
			{
				return {};
			}
			virtual wxAny GetEditorValue(const Column& column) const
			{
				return {};
			}
			virtual bool SetValue(const wxAny& value, Column& column)
			{
				return false;
			}
			
			virtual bool GetAttributes(CellAttributes& attributes, const CellState& cellState, const Column& column) const
			{
				return false;
			}
			virtual int GetRowHeight() const
			{
				return 0;
			}

			virtual bool Compare(const Node& other, const Column& column) const
			{
				return false;
			}
		};
}
