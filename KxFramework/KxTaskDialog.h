#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxUtility.h"
#include <CommCtrl.h>

enum KxTD_Options
{
	KxTD_NONE = 0,

	KxTD_HYPERLINKS_ENABLED = 1 << 0,
	KxTD_CMDLINKS_ENABLED = 1 << 1,
	KxTD_CMDLINKS_NO_ICON = 1 << 2,
	KxTD_EXMESSAGE_IN_FOOTER = 1 << 3,
	KxTD_EXMESSAGE_EXPANDED = 1 << 4,
	KxTD_PB_SHOW = 1 << 5,
	KxTD_PB_PULSE = 1 << 6,
	KxTD_CALLBACK_TIMER = 1 << 7,
	KxTD_RB_NO_DEFAULT = 1 << 8,
	KxTD_CHB_CHECKED = 1 << 9,
	KxTD_CHB_ENABLED = 1 << 10,
	KxTD_SIZE_TO_CONTENT = 1 << 11,
};
class KX_API KxTaskDialog: public KxDialog, public KxIStdDialog
{
	public:
		typedef std::vector<TASKDIALOG_BUTTON> ButtonSpecArray;

	private:
		TASKDIALOGCONFIG m_DialogConfig = {0};
		KxTD_Options m_Options = KxTD_NONE;
		wxWindow* m_Parent = nullptr;
		HWND m_Handle = nullptr;
		HRESULT m_Result = S_FALSE;

		wxIcon m_MainIcon;
		KxIconType m_MainIconID = KxICON_INFORMATION;
		wxIcon m_FooterIcon;
		KxIconType m_FooterIconID = KxICON_NONE;

		wxString m_Title;
		wxString m_Caption;
		wxString m_ExMessage;
		wxString m_Message;
		wxString DialogExpandButtonExpand;
		wxString DialogExpandButtonCollapse;
		wxString m_FooterMessage;
		wxString m_CheckBoxLabel;
		BOOL m_CheckBoxChecked = FALSE;
		wxWindowID m_SelectedRadioButton = KxID_NONE;

		KxStringVector m_ButtonLabels;
		std::vector<TASKDIALOG_BUTTON> m_ButtonsID;
		KxStringVector m_RadioButtonLabels;
		std::vector<TASKDIALOG_BUTTON> m_RadioButtonsID;

	private:
		static HRESULT CALLBACK EventsCallback(HWND hWnd, UINT notification, WPARAM wParam, LPARAM lParam, LONG_PTR refData);
		int ShowDialog(bool isModal = true);

		void SetStdButtonsFromWx(KxButtonType buttons);
		wxWindowID TranslateButtonID_WinToWx(int idWin) const;
		int TranslateButtonID_WxToWin(wxWindowID idWx) const;
		LPCWSTR TranslateIconIDToTDI(KxIconType id) const;

		inline LPCWSTR GetStringOrNull(const wxString& text)
		{
			return text.IsEmpty() ? nullptr : text.wc_str();
		}
		inline void SetFlagTDI(TASKDIALOG_FLAGS flag, bool set = true)
		{
			KxUtility::ModFlagRef(m_DialogConfig.dwFlags, flag, set);
		}
		inline bool IsFlagSetTDI(TASKDIALOG_FLAGS flag) const
		{
			return KxUtility::HasFlag(m_DialogConfig.dwFlags, flag);
		}

		void UpdateButtonArrays(const KxStringVector& labels, ButtonSpecArray& array);

	public:
		static const KxTD_Options DefaultStyle = KxTD_NONE;
		static const int DefaultButtons = KxBTN_OK;
		
		KxTaskDialog() {}
		KxTaskDialog(wxWindow* parent,
					 wxWindowID id,
					 const wxString& sCaption = wxEmptyString,
					 const wxString& message = wxEmptyString,
					 int buttons = DefaultButtons,
					 KxIconType nMainIcon = KxICON_NONE,
					 wxPoint pos = wxDefaultPosition,
					 wxSize size = wxDefaultSize,
					 long style = DefaultStyle
		)
		{
			Create(parent, id, sCaption, message, buttons, nMainIcon, pos, size, style);
		}
		bool Create(wxWindow* pParent,
					wxWindowID id,
					const wxString& sCaption = wxEmptyString,
					const wxString& message = wxEmptyString,
					int buttons = DefaultButtons,
					KxIconType nMainIcon = KxICON_NONE,
					wxPoint pos = wxDefaultPosition,
					wxSize size = wxDefaultSize,
					long style = DefaultStyle
		);
		virtual ~KxTaskDialog();
		virtual bool Close(bool force = false);

	public:
		bool IsOK()
		{
			return SUCCEEDED(m_Result);
		}
		virtual WXHWND GetHandle() const override
		{
			return m_Handle;
		}
		virtual bool Show(bool show = true) override;
		virtual int ShowModal();
		bool Realize()
		{
			if (GetHandle())
			{
				return SendMessageW(GetHandle(), TDM_NAVIGATE_PAGE, 0, (LPARAM)&m_DialogConfig) == S_OK;
			}
			return false;
		}

		// Options
		bool IsOptionEnabled(KxTD_Options flag) const
		{
			return m_Options & flag;
		}
		void SetOptionEnabled(KxTD_Options flag, bool isEnabled = true)
		{
			SetOptions(KxUtility::ModFlag(m_Options, flag, isEnabled));
		}
		KxTD_Options GetOptions() const
		{
			return m_Options;
		}
		void SetOptions(KxTD_Options options);

		// Icons
		virtual KxIconType GetMainIconID() const override
		{
			return m_MainIconID;
		}
		wxBitmap GetMainIcon() const override
		{
			if (m_MainIcon.IsOk())
			{
				return m_MainIcon;
			}
			else
			{
				return wxArtProvider::GetMessageBoxIcon(TranslateIconIDToWx(GetMainIconID()));
			}
		}
		void SetMainIcon(KxIconType iconID) override
		{
			if (iconID != KxICON_QUESTION)
			{
				m_MainIconID = iconID;
				m_MainIcon = wxNullIcon;

				SetFlagTDI(TDF_USE_HICON_MAIN, false);
				m_DialogConfig.hMainIcon = nullptr;
				m_DialogConfig.pszMainIcon = TranslateIconIDToTDI(m_MainIconID);
			}
			else
			{
				SetMainIcon(wxArtProvider::GetMessageBoxIcon(wxICON_QUESTION));
				m_MainIconID = iconID;
			}
		}
		void SetMainIcon(const wxBitmap& icon) override
		{
			m_MainIconID = KxICON_NONE;
			m_MainIcon.CopyFromBitmap(icon);

			SetFlagTDI(TDF_USE_HICON_MAIN, true);
			m_DialogConfig.pszMainIcon = nullptr;
			m_DialogConfig.hMainIcon = m_MainIcon.GetHICON();
		}
		
		KxIconType GetFooterIconID()
		{
			return m_FooterIconID;
		}
		wxBitmap GetFooterIcon()
		{
			if (m_FooterIcon.IsOk())
			{
				return m_FooterIcon;
			}
			else
			{
				return wxArtProvider::GetMessageBoxIcon(TranslateIconIDToWx(GetFooterIconID()));
			}
		}
		void SetFooterIcon(KxIconType iconID)
		{
			m_FooterIconID = iconID;
			m_FooterIcon = wxNullIcon;

			SetFlagTDI(TDF_USE_HICON_FOOTER, false);
			m_DialogConfig.pszFooterIcon = TranslateIconIDToTDI(m_FooterIconID);
			m_DialogConfig.hFooterIcon = nullptr;
		}
		void SetFooterIcon(const wxBitmap& icon)
		{
			m_FooterIconID = KxICON_NONE;
			m_FooterIcon.CopyFromBitmap(icon);

			SetFlagTDI(TDF_USE_HICON_FOOTER, true);
			m_DialogConfig.pszFooterIcon = nullptr;
			m_DialogConfig.hFooterIcon = m_FooterIcon.GetHICON();
		}

		// Strings
		virtual wxString GetTitle() const override
		{
			return m_Title;
		}
		virtual void SetTitle(const wxString& string = wxEmptyString) override
		{
			if (string.IsEmpty())
			{
				m_Title = wxTheApp->GetAppDisplayName();
			}
			else
			{
				m_Title = string;
			}
			m_DialogConfig.pszWindowTitle = m_Title.wc_str();
		}
		virtual wxString GetCaption() const override
		{
			return m_Caption;
		}
		virtual void SetCaption(const wxString& string) override
		{
			m_Caption = string;
			m_DialogConfig.pszMainInstruction = GetStringOrNull(m_Caption);
		}
		virtual wxString GetLabel() const override
		{
			return GetMessage();
		}
		virtual void SetLabel(const wxString& string) override
		{
			SetMessage(string);
		}

		wxString GetMessage() const
		{
			return m_Message;
		}
		void SetMessage(const wxString& string)
		{
			m_Message = string;
			m_DialogConfig.pszContent = GetStringOrNull(m_Message);
		}
		wxString GetExMessage() const
		{
			return m_ExMessage;
		}
		void SetExMessage(const wxString& string)
		{
			m_ExMessage = string;
			m_DialogConfig.pszExpandedInformation = GetStringOrNull(m_ExMessage);
		}
		wxString GetFooter() const
		{
			return m_FooterMessage;
		}
		void SetFooter(const wxString& string)
		{
			m_FooterMessage = string;
			m_DialogConfig.pszFooter = GetStringOrNull(m_FooterMessage);
		}
		wxString GetCheckBoxLabel() const
		{
			return m_CheckBoxLabel;
		}
		void SetCheckBoxLabel(const wxString& text)
		{
			m_CheckBoxLabel = text;
			m_DialogConfig.pszVerificationText = GetStringOrNull(m_CheckBoxLabel);
		}

		// Buttons
		virtual void SetDefaultButton(wxWindowID id) override
		{
			m_DialogConfig.nDefaultButton = TranslateButtonID_WxToWin(id);
		}
		void SetDefaultRadioButton(wxWindowID id)
		{
			m_DialogConfig.nDefaultRadioButton = id;
		}
		
		virtual KxStdDialogControl GetButton(wxWindowID id) const override
		{
			return id;
		}
		virtual KxStdDialogControl AddButton(wxWindowID id, const wxString& label = wxEmptyString, bool prepend = false) override;
		KxStdDialogControl AddRadioButton(wxWindowID id, const wxString& label = wxEmptyString);
		
		bool IsCheckBoxChecked() const
		{
			return m_CheckBoxChecked;
		}
		wxWindowID GetSelectedRadioButton() const
		{
			return m_SelectedRadioButton;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxTaskDialog);
};
