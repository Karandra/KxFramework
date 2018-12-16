#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewEditor.h"
#include "KxFramework/DataView/KxDataViewComboBoxEditor.h"
#include "KxFramework/KxDataViewEvent.h"
class KxDataViewComboBox;

KX_DECLARE_EVENT(KxEVT_DVR_DVCOMBOBOX_INIT_CONTROL, KxDataViewEvent);

class KX_API KxDataViewDVComboBoxEditor: public wxEvtHandler, public KxDataViewEditor, public KxDataViewComboBoxEditorBase
{
	private:
		mutable KxDataViewComboBox* m_ComboBox = NULL;
		mutable KxDataViewItem m_Selection;
		KxDataViewModel* m_Model = NULL;
		bool m_OwnModel = false;

		KxDataViewCtrlStyles m_DataViewStyle = KxDataViewCtrl::DefaultStyle;

	protected:
		virtual void OnInitControl(const wxAny& value);
		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;

	public:
		virtual ~KxDataViewDVComboBoxEditor();

	public:
		KxDataViewComboBox* GetComboBox() const
		{
			return m_ComboBox;
		}
		KxDataViewItem GetSelection() const
		{
			return m_Selection;
		}

		KxDataViewModel* GetModel() const
		{
			return m_Model;
		}
		void SetModel(KxDataViewModel* model)
		{
			m_Model = model;
			m_OwnModel = false;
		}
		void AssignModel(KxDataViewModel* model)
		{
			m_Model = model;
			m_OwnModel = true;
		}

		KxDataViewCtrlStyles GetControlStyle() const
		{
			return m_DataViewStyle;
		}
		void SetControlStyle(KxDataViewCtrlStyles style)
		{
			m_DataViewStyle = style;
		}
};
