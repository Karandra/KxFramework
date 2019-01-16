#include "KxStdAfx.h"
#include "ToggleRenderer.h"
#include "KxFramework/DataView2/View.h"
#include "KxFramework/DataView2/Node.h"
#include "KxFramework/DataView2/Column.h"

namespace Kx::DataView2
{
	bool ToggleRenderer::GetValueAsToggleState(const wxAny& value, ToggleState& state)
	{
		// Check for 'ToggleState'
		{
			ToggleState tempState = ToggleState::None;
			if (value.CheckType<ToggleState>() && value.GetAs(&tempState))
			{
				switch (tempState)
				{
					case ToggleState::Checked:
					case ToggleState::Unchecked:
					case ToggleState::Indeterminate:
					{
						state = tempState;
						return true;
					}
				};
			}
		}

		// Check for 'wxCheckBoxState'
		{
			wxCheckBoxState tempState = (wxCheckBoxState)-1; // Should be invalid 'wxCheckBoxState' value
			if (value.CheckType<wxCheckBoxState>() && value.GetAs(&tempState))
			{
				switch (tempState)
				{
					case wxCHK_CHECKED:
					{
						state = ToggleState::Checked;
						return true;
					}
					case wxCHK_UNCHECKED:
					{
						state = ToggleState::Unchecked;
						return true;
					}
					case wxCHK_UNDETERMINED:
					{
						state = ToggleState::Indeterminate;
						return true;
					}
				};
			}
		}

		// Check for bool
		{
			bool checked = false;
			if (value.CheckType<bool>() && value.GetAs(&checked))
			{
				state = checked ? ToggleState::Checked : ToggleState::Unchecked;
				return true;
			}
		}
		return false;
	}
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
				nextState = Is3StateAllowed() ? ToggleState::Indeterminate : ToggleState::Unchecked;
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

namespace Kx::DataView2
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
		m_Value = ToggleValue();
		if (!value.GetAs(&m_Value))
		{
			m_Value.SetType(GetDefaultToggleType());

			ToggleState state = ToggleState::None;
			if (GetValueAsToggleState(value, state))
			{
				m_Value.SetState(state);
			}
			else
			{
				m_Value.SetState(GetDefaultToggleState());
				return false;
			}
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
		return GetRenderEngine().GetToggleSize();
	}
}
