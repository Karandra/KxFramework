#pragma once
#include "Common.h"
#include <KxFramework/KxWithOptions.h>

namespace Kx::DataView2
{
	class CellState
	{
		private:
			enum class Value: uint32_t
			{
				None = 0,
				Selected = 1 << 0,
				Highlighted = 1 << 1,
				DropTarget = 1 << 2,
			};

		private:
			KxWithOptions<Value, Value::None> m_Value;

		public:
			bool IsSelected() const
			{
				return m_Value.IsOptionEnabled(Value::Selected);
			}
			bool IsHighlighted() const
			{
				return m_Value.IsOptionEnabled(Value::Highlighted);
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
			CellState& SetHighlighted()
			{
				m_Value.SetOptionEnabled(Value::Highlighted);
				return *this;
			}
			CellState& SetDropTarget()
			{
				m_Value.SetOptionEnabled(Value::DropTarget);
				return *this;
			}
	};
}
