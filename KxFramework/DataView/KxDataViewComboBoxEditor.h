#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithImageList.h"
#include "KxFramework/DataView/KxDataViewEditor.h"

class KX_API KxDataViewComboBoxEditorBase
{
	private:
		int m_MaxVisibleItems = -1;
		bool m_IsControlEditable = false;
		bool m_EndEditOnCloseup = false;

	public:
		int GetMaxVisibleItems() const
		{
			return m_MaxVisibleItems;
		}
		void SetMaxVisibleItems(int max = -1)
		{
			m_MaxVisibleItems = max;
		}
		
		bool ShouldEndEditOnCloseup() const
		{
			return m_EndEditOnCloseup;
		}
		void EndEditOnCloseup(bool value)
		{
			m_EndEditOnCloseup = value;
		}

		bool IsEditable() const
		{
			return m_IsControlEditable;
		}
		void SetEditable(bool isEditable)
		{
			m_IsControlEditable = isEditable;
		}
};

class KX_API KxDataViewComboBoxEditor: public KxDataViewEditor, public KxDataViewComboBoxEditorBase, public KxWithImageList
{
	private:
		KxStringVector m_Items;
		KxIntVector m_BitmapIndexes;
		bool m_AlwaysUseStringSelection = false;
		bool m_UseBitmap = false;

	protected:
		virtual KxDataViewEditorControlHandler* CreateControlHandler() override;

		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;

	public:
		const KxStringVector& GetItems() const
		{
			return m_Items;
		}
		void SetItems(const KxStringVector& tValues)
		{
			m_Items = tValues;
		}
		void ClearItems()
		{
			m_Items.clear();
		}

		const KxIntVector& GetBitmapIndexes() const
		{
			return m_BitmapIndexes;
		}
		void SetBitmapIndexes(const KxIntVector& tValues)
		{
			m_BitmapIndexes = tValues;
		}
		void ClearBitmapIndexes()
		{
			m_BitmapIndexes.clear();
		}

	public:
		bool ShouldAlwaysUseStringSelection() const
		{
			return m_AlwaysUseStringSelection;
		}
		void AlwaysUseStringSelection(bool use)
		{
			m_AlwaysUseStringSelection = use;
		}

		bool IsBitmapsEnabled() const
		{
			return m_UseBitmap;
		}
		void SetBitmapsEnabled(bool enabled)
		{
			m_UseBitmap = enabled;
		}
};

class KX_API KxDataViewComboBoxEditorControlHandler: public KxDataViewEditorControlHandler
{
	protected:
		virtual void DoOnKillFocus(wxFocusEvent& event) override;

	public:
		KxDataViewComboBoxEditorControlHandler(KxDataViewComboBoxEditor* editor, wxWindow* control)
			:KxDataViewEditorControlHandler(editor, control)
		{
		}
};
