#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewDVComboBoxEditor.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/KxDataViewComboBox.h"

wxDEFINE_EVENT(KxEVT_DVR_DVCOMBOBOX_INIT_CONTROL, KxDataViewEvent);

void KxDataViewDVComboBoxEditor::OnInitControl(const wxAny& value)
{
	KxDataViewEvent event(KxEVT_DVR_DVCOMBOBOX_INIT_CONTROL);
	GetMainWindow()->CreateEventTemplate(event, m_Selection, GetColumn());

	event.SetEventObject(this);
	event.SetColumn(GetColumn());
	event.GetValue() = value;
	ProcessEvent(event);
}

wxWindow* KxDataViewDVComboBoxEditor::CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value)
{
	m_Selection.MakeNull();
	m_ComboBox = new KxDataViewComboBox();
	m_ComboBox->SetOptionEnabled(KxDVCB_OPTION_ALT_POPUP_WINDOW, false);
	m_ComboBox->SetOptionEnabled(KxDVCB_OPTION_DISMISS_ON_SELECT, true);
	m_ComboBox->SetOptionEnabled(KxDVCB_OPTION_FORCE_GET_STRING_VALUE_ON_DISMISS, false);
	m_ComboBox->SetDataViewFlags(m_DataViewStyle);
	m_ComboBox->Create(parent, wxID_NONE, (IsEditable() ? 0 : wxCB_READONLY), GetValidator());

	wxComboCtrl* editorContainer = m_ComboBox->GetComboControl();
	editorContainer->SetPosition(cellRect.GetTopLeft());
	//editorContainer->Move(cellRect.GetRight() - editorContainer->GetRect().GetWidth(), wxDefaultCoord);
	editorContainer->SetInitialSize(cellRect.GetSize());
	
	m_ComboBox->Bind(KxEVT_DATAVIEW_ITEM_SELECTED, [this](KxDataViewEvent& event)
	{
		event.StopPropagation();
		event.Skip();

		m_Selection = event.GetItem();
		if (m_ComboBox->IsOptionEnabled(KxDVCB_OPTION_DISMISS_ON_SELECT))
		{
			// This will cause dismiss
			GetView()->SetFocus();
		}
	});
	m_ComboBox->Bind(KxEVT_DVCB_GET_STRING_VALUE, [this](KxDataViewEvent& event)
	{
		if (m_Model)
		{
			wxAny label;
			m_Model->GetValue(label, m_Selection, NULL);
			event.SetString(label.As<wxString>());
		}
		event.StopPropagation();
		event.Skip();
	});

	if (GetMaxVisibleItems() != -1)
	{
		m_ComboBox->ComboSetMaxVisibleItems(GetMaxVisibleItems());
	}

	if (m_Model)
	{
		m_ComboBox->AssociateModel(m_Model);
	}
	OnInitControl(value);
	m_ComboBox->ComboRefreshLabel();

	if (ShouldEndEditOnCloseup())
	{
		m_ComboBox->Bind(wxEVT_COMBOBOX_CLOSEUP, [this](wxCommandEvent& event)
		{
			EndEdit();
			event.Skip();
		});
	}
	return editorContainer;
}
bool KxDataViewDVComboBoxEditor::GetValueFromEditor(wxWindow* control, wxAny& value) const
{
	value = m_Selection;

	m_Selection.MakeNull();
	m_ComboBox->AssociateModel(NULL);
	m_ComboBox = NULL;
	return true;
}

KxDataViewDVComboBoxEditor::~KxDataViewDVComboBoxEditor()
{
	if (m_OwnModel)
	{
		while (m_Model->GetRefCount() > 1)
		{
			m_Model->DecRef();
		}
		m_Model->DecRef();
	}
}
