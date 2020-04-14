#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxComboPopup.h"
#include "KxFramework/DataView2/DataView2.h"
#include "Kx/General/WithOptions.h"
class KX_API KxComboControl;
class KX_API KxPanel;

namespace KxDataView2
{
	KxEVENT_DECLARE_GLOBAL(Event, DVCB_GET_STRING_VALUE);
	KxEVENT_DECLARE_GLOBAL(Event, DVCB_SET_STRING_VALUE);

	enum CBOptions
	{
		KxDVCB_OPTION_NONE = 0,
		KxDVCB_OPTION_DISMISS_ON_SELECT = 1 << 0,
		KxDVCB_OPTION_FORCE_GET_STRING_VALUE_ON_DISMISS = 1 << 1,
		KxDVCB_OPTION_ALT_POPUP_WINDOW = 1 << 2,
		KxDVCB_OPTION_HORIZONTAL_SIZER = 1 << 3,

		KxDVCB_OPTION_DEFAULT = KxDVCB_OPTION_FORCE_GET_STRING_VALUE_ON_DISMISS|KxDVCB_OPTION_ALT_POPUP_WINDOW,
	};
}
namespace KxDataView2
{
	class KX_API ComboBoxCtrl:
		public View,
		public KxComboPopup,
		public KxFramework::WithOptions<CBOptions, KxDVCB_OPTION_DEFAULT>
	{
		protected:
			CtrlStyle m_DataViewFlags = DefaultDataViewStyle;
			KxPanel* m_BackgroundWindow = nullptr;
			wxBoxSizer* m_Sizer = nullptr;

			KxComboControl* m_ComboCtrl = nullptr;
			wxEvtHandler m_EvtHandler_DataView;
			wxEvtHandler m_EvtHandler_ComboCtrl;
			Node* m_Selection = nullptr;
			bool m_PopupWinEvtHandlerPushed = false;
			int m_MaxVisibleItems = -1;

		private:
			int CalculateItemsHeight() const;
			void UpdatePopupHeight();

		protected:
			void OnInternalIdle() override;
			void OnSelectItem(Event& event);
			void OnScroll(wxMouseEvent& event);

			bool FindItem(const wxString& value, wxString* pTrueItem = nullptr) override;
			void OnPopup() override;
			void OnDismiss() override;
			void OnDoShowPopup() override;
			bool Create(wxWindow* window) override;

		public:
			static constexpr int DefaultStyle = wxCB_READONLY;
			static constexpr CtrlStyle DefaultDataViewStyle = CtrlStyle::Default;

			ComboBoxCtrl() = default;
			ComboBoxCtrl(wxWindow* parent,
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
			~ComboBoxCtrl();

		public:
			 wxWindow* GetControl() override;
			 wxComboCtrl* GetComboControl();
			 wxWindow* ComboGetBackgroundWindow();
			 wxString GetStringValue() const override;
			 void SetStringValue(const wxString& value) override;

			 CtrlStyle GetDataViewFlags() const
			 {
				 return m_DataViewFlags;
			 }
			 void SetDataViewFlags(CtrlStyle flags)
			 {
				 m_DataViewFlags = flags;
			 }

			 void ComboPopup();
			 void ComboDismiss();
			 void ComboRefreshLabel();

			 int ComboGetMaxVisibleItems() const;
			 void ComboSetMaxVisibleItems(int count);

			 void ComboSetPopupExtents(int nLeft = 0, int nRight = 0);
			 void ComboSetPopupMinWidth(int width = -1);
			 void ComboSetPopupAnchor(wxDirection nSide = (wxDirection)0);

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(ComboBoxCtrl);
	};
}
