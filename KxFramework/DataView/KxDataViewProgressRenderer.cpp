#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewProgressRenderer.h"

bool KxDataViewProgressRenderer::SetValue(const wxAny& value)
{
	m_Value = KxDataViewProgressValue();
	if (!value.GetAs<KxDataViewProgressValue>(&m_Value))
	{
		int position = -1;
		if (value.GetAs<int>(&position) && position >= 0)
		{
			m_Value.SetPosition(position);
		}
		else
		{
			m_Value.SetPosition(0);
			return false;
		}
	}
	return true;
}

void KxDataViewProgressRenderer::DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState)
{
	if (m_Value.HasPosition())
	{
		DoDrawProgressBar(cellRect, cellState, m_Value.GetPosition(), m_Value.GetRange(), static_cast<KxDataViewRenderer::ProgressBarState>(m_Value.GetState()));
	}
	if (m_Value.HasText())
	{
		DoDrawText(cellRect, cellState, m_Value.GetText());
	}
}
wxSize KxDataViewProgressRenderer::GetCellSize() const
{
	// Return 'wxDefaultCoord' because a progress bar fits any width. Unlike most renderers,
	// it doesn't have a "good" width for the content. This makes it
	// grow to the whole column, which is pretty much always the desired
	// behaviour. Keep the height fixed so that the progress bar isn't too fat.

	switch (m_SizeOption)
	{
		case KxDVR_PROGRESS_HEIGHT_AUTO:
		{
			return wxSize(wxDefaultCoord, std::max(GetView()->FromDIP(17), GetView()->GetCharHeight() + 2));
		}
		case KxDVR_PROGRESS_HEIGHT_FIT:
		{
			return wxSize(wxDefaultCoord, GetView()->GetUniformRowHeight() - GetView()->FromDIP(4));
		}
	};
	return KxDataViewRenderer::GetCellSize();
}
