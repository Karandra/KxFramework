#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewComboBoxEditor.h"
#include "KxFramework/KxComboBox.h"
#include "KxFramework/KxBitmapComboBox.h"

KxDataViewEditorControlHandler* KxDataViewComboBoxEditor::CreateControlHandler()
{
	return new KxDataViewComboBoxEditorControlHandler(this, GetEditorControl());
}

wxWindow* KxDataViewComboBoxEditor::CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
{
	int comboBoxStyles = wxCB_DROPDOWN|wxTE_PROCESS_ENTER|(IsEditable() ? 0 : wxCB_READONLY);
	wxComboBox* editor = nullptr;
	if (m_UseBitmap)
	{
		KxBitmapComboBox* bitmapEditor = new KxBitmapComboBox(parent, wxID_NONE, wxEmptyString, cellRect.GetPosition(), cellRect.GetSize(), comboBoxStyles, GetValidator());
		bitmapEditor->SetImageList(GetImageList());

		editor = bitmapEditor;
	}
	else
	{
		editor = new KxComboBox(parent, wxID_NONE, wxEmptyString, cellRect.GetPosition(), cellRect.GetSize(), comboBoxStyles, GetValidator());
	}
	editor->SetMaxSize(cellRect.GetSize());
	
	if (m_UseBitmap)
	{
		for (size_t i = 0; i < m_Items.size(); i++)
		{
			static_cast<KxBitmapComboBox*>(editor)->AddItem(m_Items[i], i < m_BitmapIndexes.size() ? m_BitmapIndexes[i] : NO_IMAGE);
		}
	}
	else
	{
		editor->Set(m_Items);
	}

	if (GetMaxVisibleItems() != -1)
	{
		if (m_UseBitmap)
		{
			static_cast<KxBitmapComboBox*>(editor)->SetVisibleItemsCount(GetMaxVisibleItems());
		}
		else
		{
			static_cast<KxComboBox*>(editor)->SetVisibleItemsCount(GetMaxVisibleItems());
		}		
	}

	if (IsEditable())
	{
		editor->ChangeValue(value.As<wxString>());

		// Select the text in the control and place the cursor at the end
		editor->SetInsertionPointEnd();
		editor->SelectAll();
	}
	else
	{
		editor->SetSelection(value.As<int>());
		editor->Popup();
	}

	if (ShouldEndEditOnCloseup())
	{
		editor->Bind(wxEVT_COMBOBOX_CLOSEUP, [this](wxCommandEvent& event)
		{
			EndEdit();
			event.Skip();
		});
	}
	return editor;
}
bool KxDataViewComboBoxEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	KxComboBox* editor = static_cast<KxComboBox*>(control);

	if (IsEditable())
	{
		value = editor->GetValue();
	}
	else
	{
		if (ShouldAlwaysUseStringSelection())
		{
			value = editor->GetStringSelection();
		}
		else
		{
			value = editor->GetSelection();
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void KxDataViewComboBoxEditorControlHandler::DoOnKillFocus(wxFocusEvent& event)
{
	event.Skip();
	if (!IsFinished())
	{
		if (static_cast<KxDataViewComboBoxEditor*>(GetEditor())->IsEditable())
		{
			// If focus was set to internal Edit control or to itself (?) don't finish editing.
			wxWindow* window = wxWindow::FindFocus();
			if (window)
			{
				if (window == GetEditorControl() || ::GetAncestor(GetEditorControl()->GetHandle(), GW_CHILD) == window->GetHandle())
				{
					return;
				}
			}
		}

		SetFinished(true);
		EndEdit();
	}
}
