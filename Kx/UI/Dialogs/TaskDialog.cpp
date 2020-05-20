#include "stdafx.h"
#include "TaskDialog.h"
#include "Private/TaskDialogNativeInfo.h"
#include "Kx/Localization/Common.h"
#include "Kx/System/DynamicLibrary.h"
#include "Kx/Drawing/Common.h"
#include <CommCtrl.h>
#include "Kx/System/UndefWindows.h"

#pragma warning(disable: 4302) // 'reinterpret_cast': truncation from 'void *' to 'int'
#pragma warning(disable: 4311) // 'reinterpret_cast': pointer truncation from 'void *' to 'int'

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(TaskDialog, StdDialog);

	HResult TaskDialog::OnDialogEvent(void* windowHandle, uint32_t notification, void* wParam, void* lParam)
	{
		switch (notification)
		{
			case TDN_CREATED:
			{
				m_Handle = windowHandle;
				break;
			}
			case TDN_DESTROYED:
			{
				m_Handle = nullptr;
				Close(true);
				break;
			}
			case TDN_HYPERLINK_CLICKED:
			case TDN_BUTTON_CLICKED:
			case TDN_RADIO_BUTTON_CLICKED:
			case TDN_VERIFICATION_CLICKED:
			{
				wxNotifyEvent event(wxEVT_NULL, GetId());
				event.SetEventObject(this);

				switch (notification)
				{
					case TDN_HYPERLINK_CLICKED:
					{
						event.SetEventType(wxEVT_TEXT_URL);
						event.SetString(reinterpret_cast<const wchar_t*>(lParam));
						break;
					}
					case TDN_BUTTON_CLICKED:
					{
						event.SetEventType(EvtButton);
						event.SetId(m_NativeInfo->TranslateButtonIDFromNative(reinterpret_cast<int>(wParam)));
						break;
					}
					case TDN_RADIO_BUTTON_CLICKED:
					{
						event.SetEventType(wxEVT_RADIOBUTTON);
						event.SetId(reinterpret_cast<int>(wParam));
						break;
					}
					case TDN_VERIFICATION_CLICKED:
					{
						event.SetEventType(wxEVT_CHECKBOX);
						event.SetInt(wParam != nullptr); // Is checked
						break;
					}
				};

				HandleWindowEvent(event);
				return event.IsAllowed() ? HResult::Success() : HResult::False();
			};
		};

		return HResult::Success();
	}
	wxWindowID TaskDialog::DoShowDialog(bool isModal)
	{
		m_NativeInfo->Realize();
		return m_NativeInfo->ShowDialog(isModal);
	}
	void TaskDialog::OnClose(wxCloseEvent& event)
	{
		if (m_Handle)
		{
			if (!event.CanVeto())
			{
				::DestroyWindow(reinterpret_cast<HWND>(m_Handle));
			}
			else
			{
				::EndDialog(reinterpret_cast<HWND>(m_Handle), wxID_CANCEL);
			}
			m_Handle = nullptr;
		}
	}

	void TaskDialog::DoSetRange(int range)
	{
		m_ProgressRange = range;

		if (m_Handle)
		{
			::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_SET_ELEMENT_TEXT, 0, Utility::IntFromLowHigh<DWORD, WORD, WORD>(0, range));
		}
	}
	void TaskDialog::DoSetValue(int value)
	{
		const bool wasPulsing = m_Style & TaskDialogStyle::ProgressBarPulse;
		Utility::RemoveFlagRef(m_Style, TaskDialogStyle::ProgressBarPulse);
		m_ProgressPos = value;

		if (m_Handle)
		{
			if (wasPulsing)
			{
				::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_SET_PROGRESS_BAR_MARQUEE, FALSE, 0);
			}
			::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_SET_PROGRESS_BAR_POS, value, 0);
		}
	}
	void TaskDialog::DoPulse()
	{
		Utility::AddFlagRef(m_Style, TaskDialogStyle::ProgressBarPulse);

		if (m_Handle)
		{
			::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_SET_PROGRESS_BAR_MARQUEE, TRUE, m_ProgressPulseInterval.GetMilliseconds());
		}
	}

	TaskDialog::TaskDialog() = default;
	TaskDialog::TaskDialog(wxWindow* parent,
						   wxWindowID id,
						   String caption,
						   String message,
						   StdButton buttons,
						   StdIcon mainIcon,
						   TaskDialogStyle style
	)
	{
		Create(parent, id, std::move(caption), std::move(message), buttons, mainIcon, style);
	}
	bool TaskDialog::Create(wxWindow* parent,
							wxWindowID id,
							String caption,
							String message,
							StdButton buttons,
							StdIcon mainIcon,
							TaskDialogStyle style
	)
	{
		if (Dialog::Create(m_Parent, id, caption, wxDefaultPosition, wxDefaultSize, EnumClass::Combine<DialogStyle>(style)))
		{
			m_Style = style;
			m_Parent = wxGetTopLevelParent(parent);
			m_Caption = std::move(caption);
			m_Message = std::move(message);

			SetDefaultTitle();
			SetMainIcon(mainIcon);
			SetReturnCode(wxID_CANCEL);
			SetStandardButtons(buttons);

			Bind(wxEVT_CLOSE_WINDOW, &TaskDialog::OnClose, this);
			return true;
		}
		return false;
	}
	TaskDialog::~TaskDialog()
	{
	}

	bool TaskDialog::Show(bool show)
	{
		// Create modeless-like dialog
		if (show && !m_Handle)
		{
			DoShowDialog(false);
		}
		return ShowNativeWindow(this, show);
	}
	int TaskDialog::ShowModal()
	{
		return DoShowDialog(true);
	}
	
	bool TaskDialog::Realize()
	{
		m_NativeInfo->Realize();
		return m_NativeInfo->UpdateWindowUI();
	}
	void TaskDialog::UpdateWindowUI(long flags)
	{
		m_NativeInfo->UpdateWindowUI();
		Dialog::UpdateWindowUI(flags);
	}

	StdIcon TaskDialog::GetMainIconID() const
	{
		return m_MainIconID;
	}
	wxBitmap TaskDialog::GetMainIcon() const
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
	void TaskDialog::SetMainIcon(StdIcon iconID)
	{
		if (iconID == StdIcon::Question)
		{
			// Windows doesn't allow to show a question icon using icon ID for the main icon
			// but if we really want to use this icon nothing should stop us!
			SetMainIcon(wxArtProvider::GetMessageBoxIcon(wxICON_QUESTION));
			m_MainIconID = iconID;
		}
		else
		{
			m_MainIconID = iconID;
			m_MainIcon = wxNullIcon;
		}

		m_NativeInfo->UpdateIcons();
		if (m_Handle)
		{
			::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_UPDATE_ICON, TDIE_ICON_MAIN, reinterpret_cast<LPARAM>(m_NativeInfo->m_DialogConfig.pszMainIcon));
		}
	}
	void TaskDialog::SetMainIcon(const wxBitmap& icon)
	{
		m_MainIconID = StdIcon::None;
		m_MainIcon = Drawing::ToIcon(icon);

		m_NativeInfo->UpdateIcons();
		if (m_Handle)
		{
			::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_UPDATE_ICON, TDIE_ICON_MAIN, reinterpret_cast<LPARAM>(m_NativeInfo->m_DialogConfig.hMainIcon));
		}
	}

	StdIcon TaskDialog::GetFooterIconID()
	{
		return m_FooterIconID;
	}
	wxBitmap TaskDialog::GetFooterIcon()
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
	void TaskDialog::SetFooterIcon(StdIcon iconID)
	{
		m_FooterIconID = iconID;
		m_FooterIcon = wxNullIcon;

		m_NativeInfo->UpdateIcons();
		if (m_Handle)
		{
			::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_UPDATE_ICON, TDIE_ICON_FOOTER, reinterpret_cast<LPARAM>(m_NativeInfo->m_DialogConfig.pszFooterIcon));
		}
	}
	void TaskDialog::SetFooterIcon(const wxBitmap& icon)
	{
		m_FooterIconID = StdIcon::None;
		m_FooterIcon = Drawing::ToIcon(icon);

		m_NativeInfo->UpdateIcons();
		if (m_Handle)
		{
			::SendMessageW(reinterpret_cast<HWND>(m_Handle), TDM_UPDATE_ICON, TDIE_ICON_FOOTER, reinterpret_cast<LPARAM>(m_NativeInfo->m_DialogConfig.hFooterIcon));
		}
	}

	void TaskDialog::SetTitle(const wxString& string)
	{
		if (!string.IsEmpty())
		{
			m_Title = string;
			Dialog::SetTitle(string);
		}
		else
		{
			SetDefaultTitle();
		}
	}
	void TaskDialog::SetDefaultTitle()
	{
		if (wxTheApp)
		{
			m_Title = wxTheApp->GetAppDisplayName();
		}
		else
		{
			m_Title = DynamicLibrary::GetExecutingModule().GetFilePath().GetName();
		}
		Dialog::SetTitle(m_Title);
	}

	void TaskDialog::SetCaption(const wxString& string)
	{
		m_Caption = string;

		if (m_Handle)
		{
			m_NativeInfo->UpdateTextElement(TDE_MAIN_INSTRUCTION, m_Caption);
		}
		else
		{
			m_NativeInfo->UpdateText();
		}
	}
	void TaskDialog::SetMessage(String string)
	{
		m_Message = std::move(string);

		if (m_Handle)
		{
			m_NativeInfo->UpdateTextElement(TDE_CONTENT, m_Message);
		}
		else
		{
			m_NativeInfo->UpdateText();
		}
	}
	void TaskDialog::SetExMessage(String string)
	{
		m_ExMessage = std::move(string);

		if (m_Handle)
		{
			m_NativeInfo->UpdateTextElement(TDE_EXPANDED_INFORMATION, m_ExMessage);
		}
		else
		{
			m_NativeInfo->UpdateText();
		}
	}
	void TaskDialog::SetFooter(String string)
	{
		m_FooterMessage = std::move(string);

		if (m_Handle)
		{
			m_NativeInfo->UpdateTextElement(TDE_FOOTER, m_FooterMessage);
		}
		else
		{
			m_NativeInfo->UpdateText();
		}
	}
	void TaskDialog::SetCheckBoxLabel(String text)
	{
		m_CheckBoxLabel = std::move(text);

		m_NativeInfo->UpdateText();
		if (m_Handle)
		{
			m_NativeInfo->UpdateWindowUI();
		}
	}

	StdDialogControl TaskDialog::GetButton(wxWindowID id) const
	{
		auto it = std::find_if(m_Buttons.begin(), m_Buttons.end(), [&](const ButtonItem& item)
		{
			return item.ID == id;
		});
		if (it != m_Buttons.end())
		{
			return id;
		}
		return {};
	}
	StdDialogControl TaskDialog::AddButton(wxWindowID id, const String& label, bool prepend)
	{
		if (label.IsEmpty())
		{
			if (prepend)
			{
				m_Buttons.emplace(m_Buttons.begin(), ButtonItem{Localization::GetStandardLocalizedString(id), id});
			}
			else
			{
				m_Buttons.emplace_back(ButtonItem{Localization::GetStandardLocalizedString(id), id});
			}
		}
		else
		{
			if (prepend)
			{
				m_Buttons.emplace(m_Buttons.begin(), ButtonItem{label, id});
			}
			else
			{
				m_Buttons.emplace_back(ButtonItem{label, id});
			}
		}
		return id;
	}
	StdDialogControl TaskDialog::AddRadioButton(wxWindowID id, const String& label)
	{
		if (label.IsEmpty())
		{
			m_RadioButtons.emplace_back(ButtonItem{Localization::GetStandardLocalizedString(id), id});
		}
		else
		{
			m_RadioButtons.emplace_back(ButtonItem{label, id});
		}
		return id;
	}
}
