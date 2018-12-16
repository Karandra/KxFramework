#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewItem.h"
class KX_API KxDataViewCtrl;
class KX_API KxDataViewColumn;
class KX_API KxDataViewRenderer;
class KX_API KxDataViewMainWindow;
class KX_API KxDataViewEditorControlHandler;

class KX_API KxDataViewEditor: public KxDataViewColumnPart
{
	friend class KxDataViewMainWindow;
	friend class KxDataViewEditorControlHandler;

	private:
		wxWindowRef m_EditorControl;
		std::unique_ptr<wxValidator> m_Validator;
		KxDataViewItem m_Item;

	private:
		virtual bool CheckedGetValue(wxAny& value);
		virtual void DestroyEditControl();

	protected:
		wxWindow* GetEditorControl() const
		{
			return m_EditorControl ? m_EditorControl.get() : NULL;
		}
		virtual KxDataViewEditorControlHandler* CreateControlHandler();

		virtual bool BeginEdit(const KxDataViewItem& item, const wxRect& cellRect);
		virtual bool EndEdit();
		virtual void CancelEdit();

	protected:
		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) = 0;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const = 0;
		
		// Called to validate value before it will be transfered to editor control
		virtual bool ValidateEditorValueBefore(const wxAny& value) const
		{
			return true;
		}
		
		// Called to validate value after it was transfered from editor control
		virtual bool ValidateEditorValueAfter(const wxAny& value) const
		{
			return true;
		}

	public:
		KxDataViewEditor();
		virtual ~KxDataViewEditor();

	public:
		KxDataViewItem GetItem() const
		{
			return m_Item;
		}

		bool HasValidator() const
		{
			return m_Validator != NULL;
		}
		const wxValidator& GetValidator() const
		{
			return HasValidator() ? *m_Validator : wxDefaultValidator;
		}
		void SetValidator(const wxValidator& tVal)
		{
			m_Validator.reset(static_cast<wxValidator*>(tVal.Clone()));
		}

	public:
		wxDECLARE_ABSTRACT_CLASS(KxDataViewEditor);
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxDataViewEditorControlHandler: public wxEvtHandler
{
	private:
		KxDataViewEditor* m_Editor = NULL;
		wxWindow* m_EditorCtrl = NULL;

		bool m_IsFinished = false;
		bool m_SetFocusOnIdle = false;

	private:
		void OnChar(wxKeyEvent& event)
		{
			DoOnChar(event);
		}
		void OnTextEnter(wxCommandEvent& event)
		{
			DoOnTextEnter(event);
		}
		void OnKillFocus(wxFocusEvent& event)
		{
			DoOnKillFocus(event);
		}
		void OnIdle(wxIdleEvent& event)
		{
			DoOnIdle(event);
		}

	protected:
		virtual void DoOnChar(wxKeyEvent& event);
		virtual void DoOnTextEnter(wxCommandEvent& event);
		virtual void DoOnKillFocus(wxFocusEvent& event);
		virtual void DoOnIdle(wxIdleEvent& event);

		KxDataViewEditor* GetEditor() const
		{
			return m_Editor;
		}
		wxWindow* GetEditorControl() const
		{
			return m_EditorCtrl;
		}

	public:
		KxDataViewEditorControlHandler(KxDataViewEditor* editor, wxWindow* control);

	public:
		bool IsFinished() const
		{
			return m_IsFinished;
		}
		void SetFinished(bool finished)
		{
			m_IsFinished = finished;
		}

		bool ShouldSetFocusOnIdle() const
		{
			return m_SetFocusOnIdle;
		}
		void SetFocusOnIdle(bool focus = true)
		{
			m_SetFocusOnIdle = focus;
		}

		bool BeginEdit(const KxDataViewItem& item, const wxRect& cellRect)
		{
			return m_Editor->BeginEdit(item, cellRect);
		}
		bool EndEdit()
		{
			return m_Editor->EndEdit();
		}
		void CancelEdit()
		{
			m_Editor->CancelEdit();
		}
};
