#pragma once
#include "kxf/UI/Common.h"
#include "ComboPopup.h"
#include "kxf/General/WithOptions.h"
#include "kxf/UI/Controls/ComboBox.h"
#include "kxf/UI/Controls/DataView.h"

namespace kxf::UI
{
	class Panel;
	class ComboControl;
}

namespace kxf::UI::DataView
{
	using namespace kxf::UI::DataView;

	enum class ComboCtrlOption: uint32_t
	{
		None = 0,
		DismissOnSelect = 1 << 0,
		ForceGetStringOnDismiss = 1 << 1,
		AltPopupWindow = 1 << 2,
		HorizontalLayout = 1 << 3,
	};
}
namespace kxf
{
	KxDeclareFlagSet(UI::DataView::ComboCtrlOption);
}

namespace kxf::UI::DataView
{
	class KX_API ComboCtrl:
		public View,
		public ComboPopup,
		public WithOptions<ComboCtrlOption, ComboCtrlOption::ForceGetStringOnDismiss|ComboCtrlOption::AltPopupWindow>
	{
		public:
			static constexpr FlagSet<ComboBoxStyle> DefaultStyle = ComboBoxStyle::ReadOnly;
			static constexpr FlagSet<CtrlStyle> DefaultDataViewStyle = CtrlStyle::Default;

			KxEVENT_MEMBER(ItemEvent, GetStringValue);
			KxEVENT_MEMBER(ItemEvent, SetStringValue);

		protected:
			FlagSet<CtrlStyle> m_DataViewFlags = DefaultDataViewStyle;
			wxBoxSizer* m_Sizer = nullptr;
			Panel* m_BackgroundWindow = nullptr;

			ComboControl* m_ComboCtrl = nullptr;
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
			void OnSelectItem(ItemEvent& event);
			void OnScroll(wxMouseEvent& event);

			bool FindItem(const wxString& value, wxString* trueItem = nullptr) override;
			void OnPopup() override;
			void OnDismiss() override;
			void OnDoShowPopup() override;
			bool Create(wxWindow* window) override;

		public:
			ComboCtrl() = default;
			ComboCtrl(wxWindow* parent,
						 wxWindowID id,
						 FlagSet<ComboBoxStyle>  style = DefaultStyle,
						 const wxValidator& validator = wxDefaultValidator
			)
			{
				ComboCtrl::Create(parent, id, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<ComboBoxStyle>  style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);
			~ComboCtrl();

		public:
			wxWindow* GetControl() override;
			wxComboCtrl* GetComboControl();
			wxWindow* ComboGetBackgroundWindow();
			wxString GetStringValue() const override;
			void SetStringValue(const wxString& value) override;

			FlagSet<CtrlStyle> GetDataViewFlags() const
			{
				return m_DataViewFlags;
			}
			void SetDataViewFlags(FlagSet<CtrlStyle> flags)
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
			void ComboSetPopupAnchor(wxDirection nSide = static_cast<wxDirection>(0));

		public:
			wxDECLARE_DYNAMIC_CLASS_NO_COPY(ComboCtrl);
	};
}
