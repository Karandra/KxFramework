#include "KxStdAfx.h"
#include "KxFramework/KxCredentialsDialog.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include <wincred.h>
#pragma comment(lib, "CredUI.lib")

using namespace KxFramework;

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
int KxCredentialsDialog::ShowModal()
{
	m_UserName.clear();
	m_Password.Wipe();

	CREDUI_INFOW credentialInfoUI = {0};
	credentialInfoUI.cbSize = sizeof(credentialInfoUI);
	credentialInfoUI.hwndParent = m_Parent ? wxGetTopLevelParent(m_Parent)->GetHandle() : nullptr;
	credentialInfoUI.pszCaptionText = m_Caption.wc_str();
	credentialInfoUI.pszMessageText = m_Message.wc_str();
	credentialInfoUI.hbmBanner = nullptr;

	ULONG authPackage = 0;
	PVOID inAuthBlob = nullptr;
	ULONG inAuthBlobSize = 0;
	PVOID authBlob = nullptr;
	ULONG authBlobSize = 0;
	BOOL saveCredentials = m_SaveCredentials;
	Utility::CallAtScopeExit ZeroAndFreeAuthBlob([&]()
	{
		if (authBlob)
		{
			::RtlSecureZeroMemory(authBlob, authBlobSize);
			::CoTaskMemFree(authBlob);
		}
	});

	DWORD ret = ::CredUIPromptForWindowsCredentialsW(&credentialInfoUI,
													 0,
													 &authPackage,
													 inAuthBlob,
													 inAuthBlobSize,
													 &authBlob,
													 &authBlobSize,
													 m_EnableSaveCredentialsCheckBox ? &saveCredentials : nullptr,
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
		Utility::CallAtScopeExit zeroCredentials([&]()
		{
			RtlSecureZeroMemory(name, sizeof(name));
			RtlSecureZeroMemory(password, sizeof(password));
			RtlSecureZeroMemory(domain, sizeof(domain));
		});

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
			m_Password = SecretValue::FromString(password, passwordLength);
		}
	}
	return ret == ERROR_SUCCESS ? KxID_OK : KxID_CANCEL;
}
