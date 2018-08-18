#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/DataView/KxDataViewEditor.h"

void KxDataViewColumn::Init()
{
	m_Renderer->SetColumn(this);
}

void KxDataViewColumn::SetSortOrder(bool ascending)
{
	if (m_DataView)
	{
		const int index = m_DataView->GetColumnIndex(this);

		// If this column isn't sorted already, mark it as sorted
		if (!m_IsSorted)
		{
			// Now set this one as the new sort column.
			m_DataView->UseColumnForSorting(index);
			m_IsSorted = true;
		}
		m_IsSortedAscending = ascending;

		// Call this directly instead of using UpdateDisplay() as we already have
		// the column index, no need to look it up again.
		m_DataView->OnColumnChange(index);
	}
}
void KxDataViewColumn::UpdateDisplay()
{
	if (m_DataView)
	{
		m_DataView->OnColumnChange(m_DataView->GetColumnIndex(this));
	}
}

void KxDataViewColumn::DeleteEditorIfNeeded()
{
	if (m_ShouldDeleteEditor)
	{
		delete m_Editor;
	}
}
void KxDataViewColumn::DoSetEditor(KxDataViewEditor* editor)
{
	DeleteEditorIfNeeded();

	m_Editor = editor;
	if (m_Editor)
	{
		m_Editor->SetOwner(this);
	}
}

KxDataViewColumn::~KxDataViewColumn()
{
	delete m_Renderer;
	DeleteEditorIfNeeded();
}

bool KxDataViewColumn::IsEditable() const
{
	return m_Renderer->GetCellMode() & KxDATAVIEW_CELL_EDITABLE && (HasEditor() || m_DynamicEditor);
}
bool KxDataViewColumn::IsActivatable() const
{
	return m_Renderer->GetCellMode() & KxDATAVIEW_CELL_ACTIVATABLE && m_Renderer->HasActivator();
}

int KxDataViewColumn::GetWidth() const
{
	switch (m_Width)
	{
		case wxCOL_WIDTH_DEFAULT:
		{
			return KxDVC_DEFAULT_WIDTH;
		}
		case wxCOL_WIDTH_AUTOSIZE:
		{
			return m_DataView->GetBestColumnWidth(m_DataView->GetColumnIndex(this));
		}
	};
	return m_Width;
}
void KxDataViewColumn::SetWidth(int width)
{
	m_Width = width;
	if (m_Width < 0 && m_Width != KxCOL_WIDTH_DEFAULT && m_Width != KxCOL_WIDTH_AUTOSIZE)
	{
		m_Width = std::abs(m_Width);
	}
	UpdateDisplay();
}

void KxDataViewColumn::ResetSorting()
{
	m_IsSorted = false;

	if (m_DataView)
	{
		m_DataView->DontUseColumnForSorting(m_DataView->GetColumnIndex(this));
	}
	UpdateDisplay();
}

bool KxDataViewColumn::IsHotTracked() const
{
	return this == m_DataView->GetMainWindow()->GetHotTrackColumn();
}
bool KxDataViewColumn::IsCurrent() const
{
	return this == m_DataView->GetMainWindow()->GetCurrentColumn();
}
bool KxDataViewColumn::IsFirst() const
{
	return IsExposed() && m_DataView->GetColumnPosition(this) == 0;
}
bool KxDataViewColumn::IsLast() const
{
	return IsExposed() && m_DataView->GetColumnPosition(this) == m_DataView->GetVisibleColumnCount();
}

//////////////////////////////////////////////////////////////////////////
KxDataViewMainWindow* KxDataViewColumnPart::GetMainWindow() const
{
	return m_Column->GetOwner()->GetMainWindow();
}
KxDataViewCtrl* KxDataViewColumnPart::GetView() const
{
	return m_Column->GetOwner();
}
KxDataViewModel* KxDataViewColumnPart::GetModel() const
{
	return m_Column->GetOwner()->GetModel();
}
KxDataViewColumn* KxDataViewColumnPart::GetColumn() const
{
	return m_Column;
}
KxDataViewRenderer* KxDataViewColumnPart::GetRenderer() const
{
	return m_Column->GetRenderer();
}
