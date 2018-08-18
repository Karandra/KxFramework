#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewTextEditor.h"
#include "KxFramework/KxTextBox.h"

wxWindow* KxDataViewTextEditor::CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
{
	KxTextBox* editor = new KxTextBox(parent, wxID_NONE, value.As<wxString>(), cellRect.GetPosition(), cellRect.GetSize(), KxTextBox::DefaultStyle|wxTE_PROCESS_ENTER, GetValidator());

	// Adjust size of 'KxTextBox' editor to fit text, even if it means being
	// wider than the corresponding column (this is how Explorer behaves).
	const int nFittingWidth = editor->GetSizeFromTextSize(editor->GetTextExtent(editor->GetValue())).x;
	const int nCurrentWidth = editor->GetSize().x;
	const int nMaxWidth = editor->GetParent()->GetSize().x - editor->GetPosition().x;

	// Adjust size so that it fits all content. Don't change anything if the allocated
	// space is already larger than needed and don't extend wxDVC's boundaries.
	int width = std::min(std::max(nCurrentWidth, nFittingWidth), nMaxWidth);
	if (width != nCurrentWidth)
	{
		editor->SetSize(wxSize(width, -1));
	}

	// Select the text in the control and place the cursor at the end
	editor->SetInsertionPointEnd();
	editor->SelectAll();
	return editor;
}
bool KxDataViewTextEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	KxTextBox* editor = static_cast<KxTextBox*>(control);
	value = editor->GetValue();
	return true;
}
