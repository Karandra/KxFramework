#pragma once
#include "Kx/UI/Common.h"

namespace KxFramework::UI::DataView
{
	enum class CtrlStyle: uint32_t
	{
		None = 0,
		SingleSelection = 0,
		MultipleSelection = 1 << 0,
		VerticalRules = 1 << 2,
		HorizontalRules = 1 << 3,
		AlternatingRowColors = 1 << 4,
		CellFocus = 1 << 5,
		FitLastColumn = 1 << 6,
		VariableRowHeight = 1 << 7,
		NoHeader = 1 << 8,

		Default = SingleSelection|VerticalRules
	};
	enum class CtrlExtraStyle: uint32_t
	{
		None = 0,
		PlusMinusExpander = 1 << 0,
		FitLastColumnToClient = 1 << 1,
	};

	enum class ColumnStyle: uint32_t
	{
		None = 0,
		Sort = 1 << 0,
		Move = 1 << 1,
		Size = 1 << 2,
		Dropdown = 1 << 3,
		CheckBox = 1 << 4,

		Default = Move|Size,
	};
	enum class DNDOpType: uint32_t
	{
		None = 0,
		Drag = 1 << 0,
		Drop = 1 << 1,
	};
	
	class ColumnWidth final
	{
		public:
			enum Value: int
			{
				Default = -1,
				AutoSize = -2,
			};

		private:
			int m_Value = Value::AutoSize;

		private:
			bool AssignValue(int value) noexcept;

		public:
			ColumnWidth(int value = Value::AutoSize) noexcept
			{
				AssignValue(value);
			}

		public:
			bool IsSpecialValue() const noexcept
			{
				return IsDefault() || IsAutoSize();
			}
			bool IsDefault() const noexcept
			{
				return m_Value == (int)Value::Default;
			}
			bool IsAutoSize() const noexcept
			{
				return m_Value == (int)Value::AutoSize;
			}

		public:
			int GetValue() const noexcept
			{
				return m_Value;
			}
			operator int() const noexcept
			{
				return m_Value;
			}
			
			ColumnWidth& operator=(int value) noexcept
			{
				AssignValue(value);
				return *this;
			}
			ColumnWidth& operator+=(int value) noexcept
			{
				AssignValue(m_Value + value);
				return *this;
			}
			ColumnWidth& operator-=(int value) noexcept
			{
				AssignValue(m_Value - value);
				return *this;
			}
	};

	enum class UniformHeight
	{
		Default,
		ListView,
		Explorer
	};
}

namespace KxFramework
{
	Kx_DeclareFlagSet(UI::DataView::CtrlStyle);
	Kx_DeclareFlagSet(UI::DataView::CtrlExtraStyle);
	Kx_DeclareFlagSet(UI::DataView::ColumnStyle);
	Kx_DeclareFlagSet(UI::DataView::DNDOpType);
}
