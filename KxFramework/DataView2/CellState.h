#pragma once
#include "Common.h"
#include "Kx/General/OptionSet.h"

namespace KxDataView2
{
	class KX_API MainWindow;
}

namespace KxDataView2
{
	class CellState
	{
		private:
			enum class Value: uint32_t
			{
				None = 0,
				Selected = 1 << 0,
				HotTracked = 1 << 1,
				DropTarget = 1 << 2,
			};

		private:
			KxFramework::OptionSet<Value, Value::None> m_Value;

		public:
			bool IsSelected() const
			{
				return m_Value.IsEnabled(Value::Selected);
			}
			bool IsHotTracked() const
			{
				return m_Value.IsEnabled(Value::HotTracked);
			}
			bool IsDropTarget() const
			{
				return m_Value.IsEnabled(Value::DropTarget);
			}

			CellState& SetSelected()
			{
				m_Value.Enable(Value::Selected);
				return *this;
			}
			CellState& SetHotTracked()
			{
				m_Value.Enable(Value::HotTracked);
				return *this;
			}
			CellState& SetDropTarget()
			{
				m_Value.Enable(Value::DropTarget);
				return *this;
			}
	
		public:
			int ToItemState(const MainWindow* window) const;
	};
}
