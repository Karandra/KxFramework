#include "KxStdAfx.h"
#include "KxFramework/KxCredentialsDialog.h"
#include "KxFramework/KxUtility.h"
#include <wincred.h>
#pragma comment(lib, "CredUI.lib")

wxIMPLEMENT_DYNAMIC_CLASS(KxCredentialsDialog, KxStdDialog);

bool KxCredentialsDialog::Create(wxWindow* parent,
								 wxWindowID id,
								 const wxString& caption,
								 const wxString& message
)
{
	KxDialog::Create(parent, id, caption, wxDefaultPosition, wxDefaultSize, KxDialog::DefaultStyle);

	m_Parent = parent;
	m_Caption = caption;
	m_Message = message;
	return true;
}
KxCredentialsDialog::~KxCredentialsDialog()
{
}

int KxCredentialsDialog::ShowModal()
{
	m_UserName.clear();
	m_Password.Wipe();

	CREDUI_INFOW credentialInfoUI = {0};
	credentialInfoUI.cbSize = sizeof(credentialInfoUI);
	credentialInfoUI.hwndParent = m_Parent ? wxGetTopLevelParent(m_Parent)->GetHandle() : NULL;
	credentialInfoUI.pszCaptionText = m_Caption;
	credentialInfoUI.pszMessageText = m_Message;
	credentialInfoUI.hbmBanner = NULL;

	ULONG authPackage = 0;
	PVOID inAuthBlob = NULL;
	ULONG inAuthBlobSize = 0;
	PVOID authBlob = NULL;
	ULONG authBlobSize = 0;
	BOOL saveCredentials = m_SaveCredentials;

	DWORD ret = ::CredUIPromptForWindowsCredentialsW(&credentialInfoUI,
													 0,
													 &authPackage,
													 inAuthBlob,
													 inAuthBlobSize,
													 &authBlob,
													 &authBlobSize,
													 m_EnableSaveCredentialsCheckBox ? &saveCredentials : NULL,
													 CREDUIWIN_GENERIC
	);
	if (ret == ERROR_SUCCESS)
	{
		// Save check box status
		if (m_EnableSaveCredentialsCheckBox)
		{
			m_SaveCredentials = saveCredentials;
		}

		// Retrieve data
		const DWORD nameLengthMax = CREDUI_MAX_USERNAME_LENGTH + 1;
		const DWORD passwordLengthMax = CREDUI_MAX_PASSWORD_LENGTH + 1;
		const DWORD domainLengthMax = CREDUI_MAX_DOMAIN_TARGET_LENGTH + 1;
		DWORD nameLength = nameLengthMax;
		DWORD passwordLength = passwordLengthMax;
		DWORD domainLength = domainLengthMax;

		WCHAR name[nameLengthMax] = {0};
		WCHAR password[passwordLengthMax] = {0};
		WCHAR domain[domainLengthMax] = {0};

		BOOL unpacked = ::CredUnPackAuthenticationBufferW(CRED_PACK_PROTECTED_CREDENTIALS,
														  authBlob,
														  authBlobSize,
														  name,
														  &nameLength,
														  domain,
														  &domainLength,
														  password,
														  &passwordLength
		);
		if (unpacked)
		{
			m_UserName = wxString(name, nameLength);
			m_Password = KxSecretValue(wxString(password, passwordLength));
		}

		RtlSecureZeroMemory(name, sizeof(name));
		RtlSecureZeroMemory(password, sizeof(password));
	}

	if (authBlob)
	{
		::RtlSecureZeroMemory(authBlob, authBlobSize);
		::CoTaskMemFree(authBlob);
	}
	return ret == ERROR_SUCCESS ? KxID_OK : KxID_CANCEL;
}
