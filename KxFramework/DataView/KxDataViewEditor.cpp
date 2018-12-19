#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewEditor.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/KxTextBox.h"

wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewEditor, wxObject);

bool KxDataViewEditor::CheckedGetValue(wxAny& value)
{
	GetModel()->GetEditorValue(value, m_Item, GetColumn());

	// We always allow the cell to be null
	if (!value.IsNull() && !ValidateEditorValueBefore(value))
	{
		wxLogDebug("Wrong type returned from the model for editor at column %d (%s)", GetColumn()->GetID(), GetColumn()->GetTitle());

		// Don't return data of mismatching type, this could be unexpected.
		value.MakeNull();
		return false;
	}
	return true;
}
void KxDataViewEditor::DestroyEditControl()
{
	if (m_EditorControl)
	{
		// Remove our event handler first to prevent it from (recursively) calling
		// us again as it would do via a call to FinishEditing() when the editor
		// loses focus when we hide it below.
		wxEvtHandler* handler = m_EditorControl->PopEventHandler();

		// Hide the control immediately but don't delete it yet as there could be
		// some pending messages for it.
		m_EditorControl->Hide();

		wxTheApp->ScheduleForDestruction(handler);
		wxTheApp->ScheduleForDestruction(m_EditorControl);

		// Ensure that DestroyEditControl() is not called again for this control.
		m_EditorControl.Release();
	}
}

KxDataViewEditorControlHandler* KxDataViewEditor::CreateControlHandler()
{
	return new KxDataViewEditorControlHandler(this, GetEditorControl());
}

bool KxDataViewEditor::BeginEdit(const KxDataViewItem& item, const wxRect& cellRect)
{
	// Before doing anything we send an event asking if editing of this item is really wanted.
	if (GetMainWindow()->SendEditingStartedEvent(item, this))
	{
		m_Item = item;

		wxAny value;
		if (CheckedGetValue(value))
		{
			m_EditorControl = CreateEditorControl(GetView()->GetMainWindow(), cellRect, value);

			// There might be no editor control for the given item
			if (m_EditorControl)
			{
				m_EditorControl->PushEventHandler(CreateControlHandler());
				m_EditorControl->SetFocus();

				return true;
			}
		}
	}
	return false;
}
bool KxDataViewEditor::EndEdit()
{
	if (m_EditorControl)
	{
		// Try to get the value, normally we should succeed but if we fail, don't
		// return immediately, we still need to destroy the edit control.
		wxAny value;
		bool hasValue = GetValueFromEditor(m_EditorControl, value);
		
		DestroyEditControl();
		GetMainWindow()->SetFocus();

		bool valueAccepted = false;
		if (hasValue)
		{
			bool isValid = ValidateEditorValueAfter(value);
			bool isAllowed = GetMainWindow()->SendEditingDoneEvent(m_Item, this, !isValid, value);
			if (isValid && isAllowed)
			{
				GetModel()->ChangeValue(value, m_Item, GetColumn());
				valueAccepted = true;
			}
		}

		m_Item.MakeNull();
		return valueAccepted;
	}
	return false;
}
void KxDataViewEditor::CancelEdit()
{
	if (m_EditorControl)
	{
		m_Item.MakeNull();
		DestroyEditControl();
	}
}

KxDataViewEditor::KxDataViewEditor()
	:KxDataViewColumnPart(nullptr)
{
}
KxDataViewEditor::~KxDataViewEditor()
{
	if (m_EditorControl)
	{
		DestroyEditControl();
	}
}

//////////////////////////////////////////////////////////////////////////
void KxDataViewEditorControlHandler::DoOnIdle(wxIdleEvent& event)
{
	event.Skip();

	if (m_SetFocusOnIdle)
	{
		m_SetFocusOnIdle = false;
		if (wxWindow::FindFocus() != m_EditorCtrl)
		{
			m_EditorCtrl->SetFocus();
		}
	}
}
void KxDataViewEditorControlHandler::DoOnTextEnter(wxCommandEvent& event)
{
	m_IsFinished = true;
	EndEdit();
}
void KxDataViewEditorControlHandler::DoOnChar(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
		case WXK_ESCAPE:
		{
			m_IsFinished = true;
			CancelEdit();
			break;
		}
		case WXK_RETURN:
		{
			if (!event.HasAnyModifiers())
			{
				m_IsFinished = true;
				EndEdit();
				break;
			}

			// Ctrl/Alt/Shift-Enter is not handled specially
			wxFALLTHROUGH;
		}
		default:
		{
			event.Skip();
		}
	};
}
void KxDataViewEditorControlHandler::DoOnKillFocus(wxFocusEvent& event)
{
	event.Skip();

	if (!m_IsFinished)
	{
		m_IsFinished = true;
		EndEdit();
	}
}

KxDataViewEditorControlHandler::KxDataViewEditorControlHandler(KxDataViewEditor* editor, wxWindow* control)
	:m_Editor(editor), m_EditorCtrl(control)
{
	m_IsFinished = false;
	m_SetFocusOnIdle = false;

	Bind(wxEVT_IDLE, &KxDataViewEditorControlHandler::OnIdle, this);
	Bind(wxEVT_CHAR, &KxDataViewEditorControlHandler::OnChar, this);
	Bind(wxEVT_KILL_FOCUS, &KxDataViewEditorControlHandler::OnKillFocus, this);
	Bind(wxEVT_TEXT_ENTER, &KxDataViewEditorControlHandler::OnTextEnter, this);
}
