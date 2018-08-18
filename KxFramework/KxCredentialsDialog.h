#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxStdDialog.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxSecretStore.h"
#include "KxFramework/KxWinUndef.h"

class KxCredentialsDialog: public KxDialog
{
	private:
		wxWindow* m_Parent = NULL;
		wxString m_Caption;
		wxString m_Message;

		bool m_SaveCredentials = false;
		bool m_EnableSaveCredentialsCheckBox = false;

		wxString m_UserName;
		KxSecretValue m_Password;

	private:
		inline LPCWSTR GetStringOrNull(const wxString& text)
		{
			return text.IsEmpty() ? NULL : text.wc_str();
		}

	public:
		KxCredentialsDialog() {}
		KxCredentialsDialog(wxWindow* parent,
							wxWindowID id,
							const wxString& caption,
							const wxString& message
		)
		{
			Create(parent, id, caption, message);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& caption,
					const wxString& message
		);
		virtual ~KxCredentialsDialog();

	public:
		wxString GetCaption() const
		{
			return m_Caption;
		}
		void SetCaption(const wxString& value)
		{
			m_Caption = value;
		}
		
		wxString GetMessage() const
		{
			return m_Message;
		}
		void SetMessage(const wxString& value)
		{
			m_Message = value;
		}

		virtual wxString GetTitle() const override
		{
			return GetCaption();
		}
		virtual void SetTitle(const wxString& value) override
		{
			SetCaption(value);
		}
		
		virtual wxString GetLabel() const override
		{
			return GetCaption();
		}
		virtual void SetLabel(const wxString& value) override
		{
			SetCaption(value);
		}

		bool ShouldSaveCredentials() const
		{
			return m_SaveCredentials;
		}
		void SaveCredentials(bool save = true)
		{
			m_SaveCredentials = save;
		}

		bool IsSaveCredentialsCheckBoxEnabled() const
		{
			return m_EnableSaveCredentialsCheckBox;
		}
		void EnableSaveCredentialsCheckBox(bool enable = true)
		{
			m_EnableSaveCredentialsCheckBox = enable;
		}

		virtual int ShowModal();

		wxString GetUserName() const
		{
			return m_UserName;
		}
		bool GetPassword(KxSecretValue& password)
		{
			password = m_Password;
			m_Password.Wipe();
			return password.IsOk();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxCredentialsDialog);
};
