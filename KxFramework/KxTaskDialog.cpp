#include "KxStdAfx.h"
#include "KxFramework/KxTaskDialog.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxTaskDialog, KxStdDialog);

void KxTaskDialog::SetStdButtonsFromWx(KxButtonType buttons)
{
	m_DialogConfig.dwCommonButtons = 0;
	if (buttons & KxBTN_OK)
	{
		m_DialogConfig.dwCommonButtons |= TDCBF_OK_BUTTON;
	}
	if (buttons & KxBTN_YES)
	{
		m_DialogConfig.dwCommonButtons |= TDCBF_YES_BUTTON;
	}
	if (buttons & KxBTN_NO)
	{
		m_DialogConfig.dwCommonButtons |= TDCBF_NO_BUTTON;
	}
	if (buttons & KxBTN_CANCEL)
	{
		m_DialogConfig.dwCommonButtons |= TDCBF_CANCEL_BUTTON;
	}
	if (buttons & KxBTN_RETRY)
	{
		m_DialogConfig.dwCommonButtons |= TDCBF_RETRY_BUTTON;
	}
	if (buttons & KxBTN_CLOSE)
	{
		m_DialogConfig.dwCommonButtons |= TDCBF_CLOSE_BUTTON;
	}
	if (buttons & KxBTN_YES)
	{
		m_DialogConfig.dwCommonButtons |= TDCBF_YES_BUTTON;
	}
}
wxWindowID KxTaskDialog::TranslateButtonID_WinToWx(int idWin) const
{
	switch (idWin)
	{
		case IDCANCEL:
		{
			return wxID_CANCEL;
		}
		case IDNO:
		{
			return wxID_NO;
		}
		case IDYES:
		{
			return wxID_YES;
		}
		case IDOK:
		{
			return wxID_OK;
		}
		case IDRETRY:
		{
			return wxID_RETRY;
		}
		case IDCLOSE:
		{
			return wxID_CLOSE;
		}
	};
	return idWin;
}
int KxTaskDialog::TranslateButtonID_WxToWin(wxWindowID idWx) const
{
	switch (idWx)
	{
		case wxID_CANCEL:
		{
			return IDCANCEL;
		}
		case wxID_NO:
		{
			return IDNO;
		}
		case wxID_YES:
		{
			return IDYES;
		}
		case wxID_OK:
		{
			return IDOK;
		}
		case wxID_RETRY:
		{
			return IDRETRY;
		}
		case wxID_CLOSE:
		{
			return IDCLOSE;
		}
	};

	return idWx;
}
LPCWSTR KxTaskDialog::TranslateIconIDToTDI(KxIconType id) const
{
	enum class TDI_IconID: int
	{
		ICON_NONE = 0,
		ICON_QUESTION = -1,

		ICON_WARNING = 1,
		ICON_ERROR,
		ICON_INFORMATION,
		ICON_SHIELD,
		ICON_SHIELDHEADER,
		ICON_SHIELDWARNING,
		ICON_SHIELDERROR,
		ICON_SHIELDSUCCESS,
		ICON_SHIELDGRAY
	};

	if (id != KxICON_QUESTION)
	{
		if (id != KxICON_NONE)
		{
			TDI_IconID idTDI = TDI_IconID::ICON_NONE;
			switch (id)
			{
				case KxICON_INFORMATION:
				{
					idTDI = TDI_IconID::ICON_INFORMATION;
					break;
				}
				case KxICON_WARNING:
				{
					idTDI = TDI_IconID::ICON_WARNING;
					break;
				}
				case KxICON_ERROR:
				{
					idTDI = TDI_IconID::ICON_ERROR;
					break;
				}
				case KxICON_SHIELD:
				{
					idTDI = TDI_IconID::ICON_SHIELD;
					break;
				}
				case KxICON_SHIELDWARNING:
				{
					idTDI = TDI_IconID::ICON_SHIELDWARNING;
					break;
				}
				case KxICON_SHIELDERROR:
				{
					idTDI = TDI_IconID::ICON_SHIELDERROR;
					break;
				}
				case KxICON_SHIELDSUCCESS:
				{
					idTDI = TDI_IconID::ICON_SHIELDSUCCESS;
					break;
				}
				case KxICON_SHIELDGRAY:
				{
					idTDI = TDI_IconID::ICON_SHIELDGRAY;
					break;
				}
			};
			return MAKEINTRESOURCEW(-(int)idTDI);
		}
		else
		{
			return NULL;
		}
	}
	return (LPCWSTR)-1;
}

void KxTaskDialog::UpdateButtonArrays(const KxStringVector& labels, ButtonSpecArray& array)
{
	for (size_t i = 0; i < labels.size(); i++)
	{
		array[i].pszButtonText = labels[i].wc_str();
	}
}

HRESULT CALLBACK KxTaskDialog::EventsCallback(HWND hWnd, UINT notification, WPARAM wParam, LPARAM lParam, LONG_PTR refData)
{
	KxTaskDialog* self = (KxTaskDialog*)refData;
	switch (notification)
	{
		case TDN_CREATED:
		{
			self->m_Handle = hWnd;

			wxShowEvent event(wxEVT_SHOW, true);
			event.SetId(self->GetId());
			event.SetEventObject(self);
			self->HandleWindowEvent(event);
			break;
		}
		case TDN_DESTROYED:
		{
			self->m_Handle = NULL;

			wxCloseEvent event(wxEVT_CLOSE_WINDOW, self->GetId());
			event.SetEventObject(self);
			event.SetCanVeto(false);
			self->HandleWindowEvent(event);
			break;
		}

		case TDN_HYPERLINK_CLICKED:
		case TDN_BUTTON_CLICKED:
		case TDN_RADIO_BUTTON_CLICKED:
		case TDN_VERIFICATION_CLICKED:
		{
			wxNotifyEvent event(wxEVT_NULL, self->GetId());
			event.SetEventObject(self);

			switch (notification)
			{
				case TDN_HYPERLINK_CLICKED:
				{
					event.SetEventType(wxEVT_TEXT_URL);
					event.SetString((LPCWSTR)lParam);
					break;
				}
				case TDN_BUTTON_CLICKED:
				{
					event.SetEventType(KxEVT_STDDIALOG_BUTTON);
					event.SetId(self->TranslateButtonID_WinToWx(wParam));
					break;
				}
				case TDN_RADIO_BUTTON_CLICKED:
				{
					event.SetEventType(wxEVT_RADIOBUTTON);
					event.SetId(wParam);
					break;
				}
				case TDN_VERIFICATION_CLICKED:
				{
					event.SetEventType(wxEVT_CHECKBOX);
					event.SetInt(wParam != 0); // Is Checked
					break;
				}
			};

			self->HandleWindowEvent(event);
			return event.IsAllowed() ? S_OK : S_FALSE;
		};
	};

	return S_OK;
}
int KxTaskDialog::ShowDialog(bool isModal)
{
	if (isModal)
	{
		m_DialogConfig.hwndParent = m_Parent ? m_Parent->GetHandle() : NULL;
	}
	else
	{
		m_DialogConfig.hwndParent = NULL;
	}
	SetFlagTDI(TDF_CAN_BE_MINIMIZED, !isModal);
	SetFlagTDI(TDF_POSITION_RELATIVE_TO_WINDOW, m_Parent != NULL);

	UpdateButtonArrays(m_ButtonLabels, m_ButtonsID);
	UpdateButtonArrays(m_RadioButtonLabels, m_RadioButtonsID);

	// Show dialog
	int retCode = wxID_CANCEL;
	int radioButtonCode = wxID_NONE;
	m_CheckBoxChecked = IsOptionEnabled(KxTD_CHB_CHECKED);
	BOOL* checkedPtr = IsOptionEnabled(KxTD_CHB_ENABLED) ? &m_CheckBoxChecked : NULL;

	SetReturnCode(wxID_CANCEL);
	m_Result = ::TaskDialogIndirect(&m_DialogConfig, &retCode, &radioButtonCode, checkedPtr);
	if (IsOK())
	{
		SetReturnCode(TranslateButtonID_WinToWx(retCode));
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
	}

	m_SelectedRadioButton = radioButtonCode;
	return GetReturnCode();
}

bool KxTaskDialog::Create(wxWindow* parent,
						  wxWindowID id,
						  const wxString& sCaption,
						  const wxString& message,
						  int buttons,
						  KxIconType nMainIcon,
						  wxPoint pos,
						  wxSize size,
						  long style
)
{
	m_Parent = wxGetTopLevelParent(parent);
	KxDialog::Create(m_Parent, id, sCaption, pos, size, KxDialog::DefaultStyle);

	m_DialogConfig.cbSize = sizeof(TASKDIALOGCONFIG);
	m_DialogConfig.hInstance = GetModuleHandleW(NULL);
	m_DialogConfig.cxWidth = 0;
	m_DialogConfig.lpCallbackData = (LONG_PTR)this;
	m_DialogConfig.pfCallback = EventsCallback;

	SetOptions((KxTD_Options)style);
	SetStdButtonsFromWx((KxButtonType)buttons);
	SetTitle();
	SetCaption(sCaption);
	SetMessage(message);
	SetMainIcon(nMainIcon);

	return true;
}
KxTaskDialog::~KxTaskDialog()
{
}
bool KxTaskDialog::Close(bool force)
{
	bool ret = KxDialog::Close(force);
	if (ret && GetHandle())
	{
		if (force)
		{
			::DestroyWindow(m_Handle);
		}
		else
		{
			::EndDialog(m_Handle, wxID_CANCEL);
		}
	}
	return ret;
}

void KxTaskDialog::SetOptions(KxTD_Options options)
{
	m_Options = options;
	auto ToggleFlag = [this, options](TASKDIALOG_FLAGS nTDIFlag, KxTD_Options nWxeOption) -> void
	{
		SetFlagTDI(nTDIFlag, options & nWxeOption);
	};

	ToggleFlag(TDF_ENABLE_HYPERLINKS, KxTD_HYPERLINKS_ENABLED);
	ToggleFlag(TDF_USE_COMMAND_LINKS, KxTD_CMDLINKS_ENABLED);
	ToggleFlag(TDF_USE_COMMAND_LINKS_NO_ICON, KxTD_CMDLINKS_NO_ICON);
	ToggleFlag(TDF_USE_COMMAND_LINKS_NO_ICON, KxTD_CMDLINKS_NO_ICON);
	ToggleFlag(TDF_EXPAND_FOOTER_AREA, KxTD_EXMESSAGE_IN_FOOTER);
	ToggleFlag(TDF_EXPANDED_BY_DEFAULT, KxTD_EXMESSAGE_EXPANDED);
	ToggleFlag(TDF_SHOW_PROGRESS_BAR, KxTD_PB_SHOW);
	ToggleFlag(TDF_SHOW_MARQUEE_PROGRESS_BAR, KxTD_PB_PULSE);
	ToggleFlag(TDF_CALLBACK_TIMER, KxTD_CALLBACK_TIMER);
	ToggleFlag(TDF_NO_DEFAULT_RADIO_BUTTON, KxTD_RB_NO_DEFAULT);
	ToggleFlag(TDF_VERIFICATION_FLAG_CHECKED, KxTD_CHB_CHECKED);
	ToggleFlag(TDF_SIZE_TO_CONTENT, KxTD_SIZE_TO_CONTENT);
}

bool KxTaskDialog::Show(bool show)
{
	// Create modeless-like dialog
	if (show && GetHandle() == NULL)
	{
		SetFlagTDI(TDF_CAN_BE_MINIMIZED, true);
		m_DialogConfig.hwndParent = m_Parent ? m_Parent->GetHandle() : NULL;

		ShowDialog(false);
	}
	return ShowNativeWindow(this, show);
}
int KxTaskDialog::ShowModal()
{
	SetFlagTDI(TDF_CAN_BE_MINIMIZED, false);
	m_DialogConfig.hwndParent = NULL;

	return ShowDialog(true);
}

KxStdDialogControl KxTaskDialog::AddButton(wxWindowID id, const wxString& label, bool prepend)
{
	if (label.IsEmpty())
	{
		m_ButtonLabels.push_back(KxUtility::GetStandardLocalizedString(id));
	}
	else
	{
		m_ButtonLabels.push_back(label);
	}
	m_ButtonsID.push_back(TASKDIALOG_BUTTON {id, NULL});

	m_DialogConfig.pButtons = m_ButtonsID.data();
	m_DialogConfig.cButtons = m_ButtonsID.size();
	return id;
}
KxStdDialogControl KxTaskDialog::AddRadioButton(wxWindowID id, const wxString& label)
{
	if (label.IsEmpty())
	{
		m_RadioButtonLabels.push_back(KxUtility::GetStandardLocalizedString(id));
	}
	else
	{
		m_RadioButtonLabels.push_back(label);
	}
	m_RadioButtonsID.push_back(TASKDIALOG_BUTTON{id, NULL});

	m_DialogConfig.pRadioButtons = m_RadioButtonsID.data();
	m_DialogConfig.cRadioButtons = m_RadioButtonsID.size();
	return id;
}
