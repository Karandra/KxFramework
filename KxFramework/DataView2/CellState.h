#pragma once
#include "Common.h"
#include <KxFramework/KxWithOptions.h>

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
			KxWithOptions<Value, Value::None> m_Value;

		public:
			bool IsSelected() const
			{
				return m_Value.IsOptionEnabled(Value::Selected);
			}
			bool IsHotTracked() const
			{
				return m_Value.IsOptionEnabled(Value::HotTracked);
			}
			bool IsDropTarget() const
			{
				return m_Value.IsOptionEnabled(Value::DropTarget);
			}

			CellState& SetSelected()
			{
				m_Value.SetOptionEnabled(Value::Selected);
				return *this;
			}
			CellState& SetHotTracked()
			{
				m_Value.SetOptionEnabled(Value::HotTracked);
				return *this;
			}
			CellState& SetDropTarget()
			{
				m_Value.SetOptionEnabled(Value::DropTarget);
				return *this;
			}
	
		public:
			int ToItemState(const MainWindow* window) const;
	};
}
