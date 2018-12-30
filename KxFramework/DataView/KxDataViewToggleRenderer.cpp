#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewToggleRenderer.h"

bool KxDataViewToggleRenderer::GetValueAsToggleState(const wxAny& value, ToggleState& state)
{
	// Check for 'ToggleState'
	{
		ToggleState tempState = ToggleState::InvalidState;
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
wxSize KxDataViewToggleRenderer::GetToggleCellSizeNoMargins(wxWindow* window)
{
	if (window)
	{
		return wxRendererNative::Get().GetCheckBoxSize(window);
	}
	else
	{
		return wxSize(KxDVC_TOGGLE_DEFAULT_WIDTH, KxDVC_DEFAULT_RENDERER_SIZE);
	}
}
wxSize KxDataViewToggleRenderer::GetToggleCellSize(wxWindow* window)
{
	if (window)
	{
		return GetToggleCellSizeNoMargins(window) + window->FromDIP(wxSize(2, 2));
	}
	else
	{
		return wxSize(KxDVC_TOGGLE_DEFAULT_WIDTH, KxDVC_DEFAULT_RENDERER_SIZE);
	}
}
int KxDataViewToggleRenderer::DrawToggle(wxDC& dc, wxWindow* window, const wxRect& cellRect, KxDataViewCellState cellState, const KxDataViewToggleValue& value, bool cellEnabled)
{
	int flags = wxCONTROL_NONE;
	switch (value.GetState())
	{
		case ToggleState::Checked:
		{
			flags |= wxCONTROL_CHECKED;
			break;
		}
		case ToggleState::Indeterminate:
		{
			flags |= wxCONTROL_UNDETERMINED;
			break;
		}
	};
	if (cellState & KxDATAVIEW_CELL_HIGHLIGHTED)
	{
		flags |= wxCONTROL_CURRENT;
	}
	if (!cellEnabled)
	{
		flags |= wxCONTROL_DISABLED;
	}

	// Ensure that the check boxes always have at least the minimal required
	// size, otherwise DrawCheckBox() doesn't really work well. If this size is
	// greater than the cell size, the checkbox will be truncated but this is a
	// lesser evil.
	wxRect toggleRect = cellRect;
	wxSize size = toggleRect.GetSize();
	size.IncTo(GetToggleCellSize(window));
	toggleRect.SetSize(size);

	if (value.GetType() == ToggleType::CheckBox || flags & wxCONTROL_UNDETERMINED)
	{
		wxRendererNative::Get().DrawCheckBox(window, dc, toggleRect, flags);
	}
	else
	{
		wxRendererNative::Get().DrawRadioBitmap(window, dc, toggleRect, flags);
	}
	return toggleRect.GetWidth();
}
bool KxDataViewToggleRenderer::DoActivateToggle(KxDataViewToggleRendererBase* rederer, wxWindow* window, const KxDataViewItem& item, const wxRect& cellRect, ToggleState& state, const wxMouseEvent* mouseEvent)
{
	if (mouseEvent)
	{
		// Only react to clicks directly on the checkbox, not elsewhere in the same cell.
		if (!wxRect(GetToggleCellSize(window)).Contains(mouseEvent->GetPosition()))
		{
			return false;
		}
	}

	ToggleState stateNext = state;
	switch (state)
	{
		case ToggleState::Checked:
		{
			stateNext = rederer->Is3StateAllowed() ? ToggleState::Indeterminate : ToggleState::Unchecked;
			break;
		}
		case ToggleState::Unchecked:
		{
			stateNext = ToggleState::Checked;
			break;
		}
		case ToggleState::Indeterminate:
		case ToggleState::InvalidState:
		{
			stateNext = ToggleState::Unchecked;
			break;
		}
	};

	state = stateNext;
	return true;
}

bool KxDataViewToggleRenderer::OnActivateCell(const KxDataViewItem& item, const wxRect& cellRect, const wxMouseEvent* mouseEvent)
{
	ToggleState state = m_Value.GetState();
	if (DoActivateToggle(this, GetView(), item, cellRect, state, mouseEvent))
	{
		return GetModel()->ChangeValue(state, item, GetColumn());
	}
	return false;
}
bool KxDataViewToggleRenderer::SetValue(const wxAny& value)
{
	m_Value = KxDataViewToggleValue();
	if (!value.GetAs<KxDataViewToggleValue>(&m_Value))
	{
		m_Value.SetType(GetDefaultToggleType());

		ToggleState state = ToggleState::InvalidState;
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

void KxDataViewToggleRenderer::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	if (m_Value.HasType())
	{
		DrawToggle(GetDC(), GetView(), cellRect, cellState, m_Value, IsEnabled());
	}
}
wxSize KxDataViewToggleRenderer::GetCellSize() const
{
	return GetToggleCellSize(GetView());
}
