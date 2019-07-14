#include "KxStdAfx.h"
#include "TextEditor.h"
#include "KxFramework/DataView2/Renderers/TextRenderer.h"
#include "KxFramework/KxTextBox.h"

namespace KxDataView2
{
	wxWindow* TextEditor::CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
	{
		const int style = KxTextBox::DefaultStyle|wxTE_PROCESS_ENTER|(IsEditable() ? 0 : wxTE_READONLY);
		const TextValue textValue = FromAnyUsing<TextValue>(value);

		KxTextBox* editor = new KxTextBox(parent,
										  wxID_NONE,
										  textValue.GetText(),
										  cellRect.GetPosition(),
										  cellRect.GetSize(),
										  style,
										  GetValidator()
		);

		// Adjust size of 'KxTextBox' editor to fit text, even if it means being
		// wider than the corresponding column (this is how Explorer behaves).
		const int fittingWidth = editor->GetSizeFromTextSize(editor->GetTextExtent(editor->GetValue())).x;
		const int currentWidth = editor->GetSize().x;
		const int maxWidth = editor->GetParent()->GetSize().x - editor->GetPosition().x;

		// Adjust size so that it fits all content. Don't change anything if the allocated
		// space is already larger than needed and don't extend wxDVC's boundaries.
		const int width = std::min(std::max(currentWidth, fittingWidth), maxWidth);
		if (width != currentWidth)
		{
			editor->SetSize(wxSize(width, -1));
		}

		// Select the text in the control and place the cursor at the end
		editor->SetInsertionPointEnd();
		editor->SelectAll();
		return editor;
	}
	wxAny TextEditor::GetValue(wxWindow* control) const
	{
		return static_cast<KxTextBox*>(control)->GetValue();
	}
}
