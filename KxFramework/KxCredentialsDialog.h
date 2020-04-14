#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDialog.h"
#include "KxFramework/KxStdDialog.h"
#include "Kx/Crypto/SecretValue.h"

class KX_API KxCredentialsDialog: public KxDialog
{
	private:
		wxWindow* m_Parent = nullptr;
		wxString m_Caption;
		wxString m_Message;

		bool m_SaveCredentials = false;
		bool m_EnableSaveCredentialsCheckBox = false;

		wxString m_UserName;
		KxFramework::SecretValue m_Password;

	private:
		inline LPCWSTR GetStringOrNull(const wxString& text)
		{
			return text.IsEmpty() ? nullptr : text.wc_str();
		}

	public:
		KxCredentialsDialog() = default;
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

		wxString GetTitle() const override
		{
			return GetCaption();
		}
		void SetTitle(const wxString& value) override
		{
			SetCaption(value);
		}
		
		wxString GetLabel() const override
		{
			return GetCaption();
		}
		void SetLabel(const wxString& value) override
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
		KxFramework::SecretValue GetPassword()
		{
			return std::move(m_Password);
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxCredentialsDialog);
};
