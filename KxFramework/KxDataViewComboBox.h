#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include "KxFramework/KxComboPopup.h"
#include "KxFramework/KxDataView.h"
class KxComboControl;
class KxPanel;

wxDECLARE_EVENT(KxEVT_DVCB_GET_STRING_VALUE, KxDataViewEvent);
wxDECLARE_EVENT(KxEVT_DVCB_SET_STRING_VALUE, KxDataViewEvent);

enum KxDataView_CBOptions
{
	KxDVCB_OPTION_NONE = 0,
	KxDVCB_OPTION_DISMISS_ON_SELECT = 1 << 0,
	KxDVCB_OPTION_FORCE_GET_STRING_VALUE_ON_DISMISS = 1 << 1,
	KxDVCB_OPTION_ALT_POPUP_WINDOW = 1 << 2,
	KxDVCB_OPTION_HORIZONTAL_SIZER = 1 << 3,

	KxDVCB_OPTION_DEFAULT = KxDVCB_OPTION_FORCE_GET_STRING_VALUE_ON_DISMISS|KxDVCB_OPTION_ALT_POPUP_WINDOW,
};
class KxDataViewComboBox:
	public KxDataViewCtrl,
	public KxComboPopup,
	public KxWithOptions<KxDataView_CBOptions, KxDVCB_OPTION_DEFAULT>
{
	protected:
		KxDataViewCtrlStyles m_DataViewFlags = DefaultDataViewStyle;
		KxPanel* m_BackgroundWindow = NULL;
		wxBoxSizer* m_Sizer = NULL;

		KxComboControl* m_ComboCtrl = NULL;
		wxEvtHandler m_EvtHandler_DataView;
		wxEvtHandler m_EvtHandler_ComboCtrl;
		KxDataViewItem m_Selection;
		bool m_PopupWinEvtHandlerPushed = false;
		int m_MaxVisibleItems = -1;

	private:
		size_t GetItemsHeight() const;
		void UpdatePopupHeight();

	protected:
		virtual void OnInternalIdle() override;
		void OnSelectItem(KxDataViewEvent& event);
		void OnScroll(wxMouseEvent& event);

		virtual bool FindItem(const wxString& value, wxString* pTrueItem = NULL) override;
		virtual void OnPopup() override;
		virtual void OnDismiss() override;
		virtual void OnDoShowPopup() override;
		virtual bool Create(wxWindow* window) override;

	public:
		static const int DefaultStyle = wxCB_READONLY;
		static const KxDataViewCtrlStyles DefaultDataViewStyle = KxDataViewCtrl::DefaultStyle;

		KxDataViewComboBox() {}
		KxDataViewComboBox(wxWindow* parent,
							wxWindowID id,
							long style = DefaultStyle,
							const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(parent, id, style, validator);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);

	public:
		 virtual wxWindow* GetControl() override;
		 virtual wxComboCtrl* GetComboControl();
		 wxWindow* ComboGetBackgroundWindow();
		 virtual wxString GetStringValue() const override;
		 virtual void SetStringValue(const wxString& value) override;

		 KxDataViewCtrlStyles GetDataViewFlags() const
		 {
			 return m_DataViewFlags;
		 }
		 void SetDataViewFlags(KxDataViewCtrlStyles flags)
		 {
			 m_DataViewFlags = flags;
		 }

		 void ComboPopup();
		 void ComboDismiss();
		 void ComboRefreshLabel();
		 int ComboGetMaxVisibleItems() const
		 {
			 return m_MaxVisibleItems;
		 }
		 void ComboSetMaxVisibleItems(int count)
		 {
			 m_MaxVisibleItems = count;
			 UpdatePopupHeight();
		 }
		 void ComboSetPopupExtents(int nLeft = 0, int nRight = 0);
		 void ComboSetPopupMinWidth(int width = -1);
		 void ComboSetPopupAnchor(wxDirection nSide = (wxDirection)0);

	public:
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxDataViewComboBox);
};
