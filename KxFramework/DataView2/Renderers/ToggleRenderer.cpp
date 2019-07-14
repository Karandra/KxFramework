#include "KxStdAfx.h"
#include "ToggleRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"

namespace KxDataView2
{
	bool ToggleValue::FromAny(const wxAny& value)
	{
		if (!value.GetAs(this))
		{
			// Check for 'ToggleState'
			if (ToggleState state = ToggleState::None; value.CheckType<ToggleState>() && value.GetAs(&state))
			{
				switch (state)
				{
					case ToggleState::Checked:
					case ToggleState::Unchecked:
					case ToggleState::Indeterminate:
					{
						m_State = state;
						return true;
					}
				};
			}

			// Check for 'wxCheckBoxState'
			if (wxCheckBoxState state = static_cast<wxCheckBoxState>(-1); value.CheckType<wxCheckBoxState>() && value.GetAs(&state))
			{
				switch (state)
				{
					case wxCHK_CHECKED:
					{
						m_State = ToggleState::Checked;
						return true;
					}
					case wxCHK_UNCHECKED:
					{
						m_State = ToggleState::Unchecked;
						return true;
					}
					case wxCHK_UNDETERMINED:
					{
						m_State = ToggleState::Indeterminate;
						return true;
					}
				};
			}

			// Check for bool
			if (bool isChecked = false; value.CheckType<bool>() && value.GetAs(&isChecked))
			{
				m_State = isChecked ? ToggleState::Checked : ToggleState::Unchecked;
				return true;
			}

			// Check for 'ToggleType'
			if (ToggleType type = ToggleType::None; value.CheckType<ToggleType>() && value.GetAs(&type))
			{
				switch (type)
				{
					case ToggleType::None:
					case ToggleType::CheckBox:
					case ToggleType::RadioBox:
					{
						m_Type = type;
						return true;
					}
				};
			}
		}
		return false;
	}
}

namespace KxDataView2
{
	bool ToggleRendererBase::DoOnActivateCell(const wxRect& toggleRect, ToggleState& state, const wxMouseEvent* mouseEvent) const
	{
		// Only react to clicks directly on the checkbox, not elsewhere in the same cell.
		if (mouseEvent && !toggleRect.Contains(mouseEvent->GetPosition()))
		{
			return false;
		}

		ToggleState nextState = state;
		switch (state)
		{
			case ToggleState::Checked:
			{
				nextState = m_Value.Is3StateAllowed() ? ToggleState::Indeterminate : ToggleState::Unchecked;
				break;
			}
			case ToggleState::Unchecked:
			{
				nextState = ToggleState::Checked;
				break;
			}
			case ToggleState::Indeterminate:
			case ToggleState::None:
			{
				nextState = ToggleState::Unchecked;
				break;
			}
		};

		state = nextState;
		return true;
	}
}

namespace KxDataView2
{
	wxAny ToggleRenderer::OnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent)
	{
		ToggleState state = m_Value.GetState();
		if (DoOnActivateCell(GetRenderEngine().GetToggleSize(), state, mouseEvent))
		{
			return state;
		}
		return {};
	}
	bool ToggleRenderer::SetValue(const wxAny& value)
	{
		if (!m_Value.FromAny(value))
		{
			m_Value = GetDefaultState();
			return false;
		}
		return true;
	}

	void ToggleRenderer::DrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		if (m_Value.HasType())
		{
			GetRenderEngine().DrawToggle(GetGraphicsDC(), cellRect, cellState, m_Value.GetState(), m_Value.GetType());
		}
	}
	wxSize ToggleRenderer::GetCellSize() const
	{
		if (m_Value.HasType())
		{
			return GetRenderEngine().GetToggleSize();
		}
		return wxSize(0, 0);
	}
}
