#include "stdafx.h"
#include "ComboBoxEditor.h"
#include "Kx/UI/Controls/ComboBox.h"
#include "Kx/UI/Controls/BitmapComboBox.h"
#include "../Renderers/TextRenderer.h"

namespace KxFramework::UI::DataView
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

	wxWindow* ComboBoxEditor::CreateControl(wxWindow* parent, const Rect& cellRect, const wxAny& value)
	{
		const ComboBoxStyle comboBoxStyles = ComboBoxStyle::Dropdown|ComboBoxStyle::ProcessEnter|(IsEditable() ? ComboBoxStyle::None : ComboBoxStyle::ReadOnly);
		wxComboBox* editor = nullptr;

		// Create the control
		if (m_UseBitmap)
		{
			BitmapComboBox* bitmapEditor = new BitmapComboBox(parent, wxID_NONE, {}, cellRect.GetPosition(), cellRect.GetSize(), comboBoxStyles, GetValidator());
			bitmapEditor->SetImageList(GetImageList());

			editor = bitmapEditor;
		}
		else
		{
			editor = new ComboBox(parent, wxID_NONE, {}, cellRect.GetPosition(), cellRect.GetSize(), comboBoxStyles, GetValidator());
		}
		editor->SetMaxSize(cellRect.GetSize());

		// Add items
		if (m_UseBitmap)
		{
			for (size_t i = 0; i < m_Items.size(); i++)
			{
				static_cast<BitmapComboBox*>(editor)->AddItem(m_Items[i], i < m_BitmapIndexes.size() ? m_BitmapIndexes[i] : KxFramework::Drawing::InvalidImageIndex);
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
				static_cast<BitmapComboBox*>(editor)->SetVisibleItemsCount(GetMaxVisibleItems());
			}
			else
			{
				static_cast<ComboBox*>(editor)->SetVisibleItemsCount(GetMaxVisibleItems());
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
		const ComboBox* editor = static_cast<const ComboBox*>(control);

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

namespace KxFramework::UI::DataView
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
