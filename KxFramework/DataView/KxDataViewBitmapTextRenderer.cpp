#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewBitmapTextRenderer.h"
#include "KxFramework/DataView/KxDataViewBitmapRenderer.h"
#include "KxFramework/DataView/KxDataViewTextRenderer.h"

wxSize KxDataViewBitmapTextRenderer::GetBitmapMargins(wxWindow* window)
{
	return window->FromDIP(wxSize(4, 2));
}
int KxDataViewBitmapTextRenderer::DrawBitmapWithText(KxDataViewRenderer* rederer, const wxRect& cellRect, KxDataViewCellState cellState, int offsetX, KxDataViewBitmapTextValue& value)
{
	if (value.HasBitmap())
	{
		const wxBitmap& bitmap = value.GetBitmap();
		rederer->DoDrawBitmap(wxRect(cellRect.GetX() + offsetX, cellRect.GetY(), cellRect.GetWidth() - offsetX, cellRect.GetHeight()), cellState, bitmap);

		offsetX += bitmap.GetWidth() + GetBitmapMargins(rederer->GetView()).GetWidth();
	}
	if (value.HasText())
	{
		rederer->DoDrawText(cellRect, cellState, value.GetText(), offsetX);
	}
	return offsetX;
}

bool KxDataViewBitmapTextRenderer::SetValue(const wxAny& value)
{
	m_Value = KxDataViewBitmapTextValue();
	if (value.GetAs<KxDataViewBitmapTextValue>(&m_Value))
	{
		return true;
	}
	if (KxDataViewBitmapRenderer::GetValueAsBitmap(value, m_Value.GetBitmap()))
	{
		return true;
	}
	if (KxDataViewTextRenderer::GetValueAsString(value, m_Value.GetText()))
	{
		return true;
	}
	return false;
}

void KxDataViewBitmapTextRenderer::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	DrawBitmapWithText(this, cellRect, cellState, 0, m_Value);
}
wxSize KxDataViewBitmapTextRenderer::GetCellSize() const
{
	wxSize size(0, 0);
	if (m_Value.HasText())
	{
		size += DoGetTextExtent(m_Value.GetText());
	}
	if (m_Value.HasBitmap())
	{
		size.x += m_Value.GetBitmap().GetWidth() + GetBitmapMargins(GetView()).x;
		if (size.y < m_Value.GetBitmap().GetHeight())
		{
			size.y = m_Value.GetBitmap().GetHeight();
		}
	}
	return size != wxSize(0, 0) ? size : wxSize(0, KxDataViewRenderer::GetCellSize().GetHeight());
}
