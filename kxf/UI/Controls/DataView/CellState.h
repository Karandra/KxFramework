#pragma once
#include "Common.h"
#include "kxf/Drawing/IRendererNative.h"

namespace kxf::UI::DataView
{
	class MainWindow;
}

namespace kxf::UI::DataView
{
	class CellState final
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
			FlagSet<Value> m_Value;

		public:
			bool IsSelected() const
			{
				return m_Value.Contains(Value::Selected);
			}
			bool IsHotTracked() const
			{
				return m_Value.Contains(Value::HotTracked);
			}
			bool IsDropTarget() const
			{
				return m_Value.Contains(Value::DropTarget);
			}

			CellState& SetSelected()
			{
				m_Value.Add(Value::Selected);
				return *this;
			}
			CellState& SetHotTracked()
			{
				m_Value.Add(Value::HotTracked);
				return *this;
			}
			CellState& SetDropTarget()
			{
				m_Value.Add(Value::DropTarget);
				return *this;
			}

		public:
			FlagSet<NativeWidgetFlag> ToItemState(const MainWindow* window) const;
	};
}
