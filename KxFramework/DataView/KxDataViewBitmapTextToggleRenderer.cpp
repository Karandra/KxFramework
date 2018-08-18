#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewBitmapTextToggleRenderer.h"
#include "KxFramework/DataView/KxDataViewBitmapRenderer.h"
#include "KxFramework/DataView/KxDataViewTextRenderer.h"
#include "KxFramework/DataView/KxDataViewToggleRenderer.h"

bool KxDataViewBitmapTextToggleRenderer::OnActivateCell(const KxDataViewItem& item, const wxRect& cellRect, const wxMouseEvent* mouseEvent)
{
	ToggleState state = m_Value.GetState();
	if (KxDataViewToggleRenderer::DoActivateToggle(this, GetView(), item, cellRect, state, mouseEvent))
	{
		return GetModel()->ChangeValue(state, item, GetColumn());
	}
	return false;
}

bool KxDataViewBitmapTextToggleRenderer::IsEnabled() const
{
	bool IsEnabled = KxDataViewRenderer::IsEnabled();
	if (m_Value.HasType())
	{
		return IsEnabled && GetCellMode() & KxDATAVIEW_CELL_ACTIVATABLE;
	}
	return IsEnabled;
}
bool KxDataViewBitmapTextToggleRenderer::SetValue(const wxAny& value)
{
	m_Value = KxDataViewBitmapTextToggleValue();
	if (!value.GetAs<KxDataViewBitmapTextToggleValue>(&m_Value))
	{
		if (KxDataViewToggleRenderer::GetValueAsToggleState(value, m_Value.GetState()))
		{
			if (!m_Value.HasType())
			{
				m_Value.SetType(GetDefaultToggleType());
			}
			return true;
		}
		if (KxDataViewTextRenderer::GetValueAsString(value, m_Value.GetText()))
		{
			return true;
		}
		if (KxDataViewBitmapRenderer::GetValueAsBitmap(value, m_Value.GetBitmap()))
		{
			return true;
		}
	}
	return false;
}

void KxDataViewBitmapTextToggleRenderer::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	int offsetX = 0;
	int offsetFromToggle = 0;

	if (m_Value.HasType())
	{
		wxRect toggleRect(cellRect.GetPosition(), KxDataViewToggleRenderer::GetToggleCellSize(GetView()));
		toggleRect.SetHeight(cellRect.GetHeight());

		KxDataViewCellState toggleCellState = cellState;
		#if 0
		wxPoint pos = GetView()->ScreenToClient(wxGetMousePosition());
		GetView()->CalcUnscrolledPosition(pos.x, pos.y, &pos.x, &pos.y);
		pos.x += toggleRect.GetWidth() / 2;
		pos.y -= 1.5 * toggleRect.GetHeight();
		pos -= KxDataViewToggleRenderer::GetToggleCellSizeNoMargins(GetView()) / 2;

		toggleCellState = KxUtility::ModFlag(cellState, KxDATAVIEW_CELL_HIGHLIGHTED, toggleRect.Contains(pos));
		#endif

		offsetX += KxDataViewToggleRenderer::DrawToggle(GetDC(), GetView(), toggleRect, toggleCellState, m_Value, IsEnabled());
		offsetFromToggle = GetView()->FromDIP(wxSize(2, 0)).x;
	}
	if (m_Value.HasBitmap() || m_Value.HasText())
	{
		wxRect rect = cellRect;
		rect.x += offsetFromToggle;
		rect.width -= offsetFromToggle;

		KxDataViewBitmapTextRenderer::DrawBitmapWithText(this, rect, cellState, offsetX, m_Value);
	}
}
wxSize KxDataViewBitmapTextToggleRenderer::GetCellSize() const
{
	wxSize size(0, 0);
	if (m_Value.HasType())
	{
		wxSize toggleSize = KxDataViewToggleRenderer::GetToggleCellSize(GetView());
		size.x += toggleSize.x;
		if (size.y < toggleSize.y)
		{
			size.y = toggleSize.y;
		}
		size += GetView()->FromDIP(wxSize(2, 0));
	}
	if (m_Value.HasText())
	{
		wxSize textExtent = DoGetTextExtent(m_Value.GetText());
		size.x += textExtent.x;
		if (size.y < textExtent.y)
		{
			size.y = textExtent.y;
		}
	}
	if (m_Value.HasBitmap())
	{
		size.x += m_Value.GetBitmap().GetWidth() + KxDataViewBitmapTextRenderer::GetBitmapMargins(GetView()).x;
		if (size.y < m_Value.GetBitmap().GetHeight())
		{
			size.y = m_Value.GetBitmap().GetHeight();
		}
	}
	return size != wxSize(0, 0) ? size : wxSize(0, KxDataViewRenderer::GetCellSize().GetHeight());
}
