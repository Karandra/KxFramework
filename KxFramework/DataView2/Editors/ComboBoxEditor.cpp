#include "KxStdAfx.h"
#include "ComboBoxEditor.h"
#include "KxFramework/KxComboBox.h"
#include "KxFramework/KxBitmapComboBox.h"
#include "KxFramework/DataView2/Renderers/TextRenderer.h"

namespace KxDataView2
{
	void ComboBoxEditor::OnSelectItem(wxCommandEvent& event)
	{
		if (!IsEditCanceled())
		{
			EndEdit();
		}
	}
	void ComboBoxEditor::OnCloseUp(wxCommandEvent& event)
	{
		if (!IsEditFinished())
		{
			CancelEdit();
		}
	}

	EditorControlHandler* ComboBoxEditor::CreateControlHandler()
	{
		return new ComboBoxEditorControlHandler(this, GetControl());
	}

	wxWindow* ComboBoxEditor::CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
	{
		const int comboBoxStyles = wxCB_DROPDOWN|wxTE_PROCESS_ENTER|(IsEditable() ? 0 : wxCB_READONLY);
		wxComboBox* editor = nullptr;

		// Create the control
		if (m_UseBitmap)
		{
			KxBitmapComboBox* bitmapEditor = new KxBitmapComboBox(parent, wxID_NONE, {}, cellRect.GetPosition(), cellRect.GetSize(), comboBoxStyles, GetValidator());
			bitmapEditor->SetImageList(GetImageList());

			editor = bitmapEditor;
		}
		else
		{
			editor = new KxComboBox(parent, wxID_NONE, {}, cellRect.GetPosition(), cellRect.GetSize(), comboBoxStyles, GetValidator());
		}
		editor->SetMaxSize(cellRect.GetSize());

		// Add items
		if (m_UseBitmap)
		{
			for (size_t i = 0; i < m_Items.size(); i++)
			{
				static_cast<KxBitmapComboBox*>(editor)->AddItem(m_Items[i], i < m_BitmapIndexes.size() ? m_BitmapIndexes[i] : KxFramework::Drawing::InvalidImageIndex);
			}
		}
		else
		{
			editor->Set(m_Items);
		}

		// Set max visible items
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

		// Select an item
		if (int index = -1; value.CheckType<int>() && value.GetAs(&index))
		{
			editor->SetSelection(index);
		}
		else if (TextValue textValue = FromAnyUsing<TextValue>(value); textValue.HasText())
		{
			if (IsEditable())
			{
				editor->ChangeValue(textValue.GetText());

				// Select the text in the control and place the cursor at the end
				editor->SetInsertionPointEnd();
				editor->SelectAll();
			}
			else
			{
				editor->SetStringSelection(textValue.GetText());
			}
		}
		m_InitialSelection = editor->GetSelection();

		// Events
		if (ShouldEndEditOnSelect())
		{
			editor->Bind(wxEVT_COMBOBOX, &ComboBoxEditor::OnSelectItem, this);
		}
		if (ShouldEndEditOnCloseup())
		{
			editor->Bind(wxEVT_COMBOBOX, &ComboBoxEditor::OnSelectItem, this);
			editor->Bind(wxEVT_COMBOBOX_CLOSEUP, &ComboBoxEditor::OnCloseUp, this);
		}

		// Popup if needed
		if (ShouldAutoPopup())
		{
			editor->Popup();
		}
		return editor;
	}
	wxAny ComboBoxEditor::GetValue(wxWindow* control) const
	{
		const KxComboBox* editor = static_cast<const KxComboBox*>(control);

		if (ShouldAlwaysUseStringSelection())
		{
			if (IsEditable())
			{
				return editor->GetValue();
			}
			else if (int index = editor->GetSelection(); index != wxNOT_FOUND)
			{
				return editor->GetString(index);
			}
		}
		else if (int index = editor->GetSelection(); index != wxNOT_FOUND)
		{
			return index;
		}
		return {};
	}
}

namespace KxDataView2
{
	void ComboBoxEditorControlHandler::DoOnKillFocus(wxFocusEvent& event)
	{
		event.Skip();
		if (!IsFinished())
		{
			if (static_cast<ComboBoxEditor*>(GetEditor())->IsEditable())
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
}
