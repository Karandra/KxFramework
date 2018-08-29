#include "KxStdAfx.h"
#include "KxFramework/KxSecretStore.h"
#include <wincred.h>

wxString KxSecretValue::GetAsString(const wxMBConv& conv) const
{
	return wxString(m_Storage.data(), conv, m_Storage.size());
}
void KxSecretValue::Wipe()
{
	::RtlSecureZeroMemory(m_Storage.data(), m_Storage.size());
	m_Storage.clear();
}

//////////////////////////////////////////////////////////////////////////
KxSecretStore KxSecretStore::GetDefault()
{
	return KxSecretStore();
}

KxSecretStore::KxSecretStore()
{
}
KxSecretStore::~KxSecretStore()
{
}

bool KxSecretStore::Save(const wxString& serviceName, const wxString& userName, const KxSecretValue& password)
{
	if (password.GetSize() <= CRED_MAX_CREDENTIAL_BLOB_SIZE)
	{
		CREDENTIALW credentialInfo = {0};
		credentialInfo.Type = CRED_TYPE_GENERIC;
		credentialInfo.TargetName = const_cast<wchar_t*>(serviceName.wc_str());
		credentialInfo.CredentialBlobSize = password.GetSize();
		credentialInfo.CredentialBlob = static_cast<uint8_t*>(const_cast<void*>(password.GetData()));
		credentialInfo.Persist = CRED_PERSIST_LOCAL_MACHINE;
		credentialInfo.UserName = const_cast<wchar_t*>(userName.wc_str());

		return ::CredWriteW(&credentialInfo, 0);
	}
	return false;
}
bool KxSecretStore::Load(const wxString& serviceName, wxString& userName, KxSecretValue& password) const
{
	PCREDENTIALW credentialInfo = NULL;
	bool status = false;
	if (::CredReadW(serviceName.wc_str(), CRED_TYPE_GENERIC, 0, &credentialInfo))
	{
		userName = credentialInfo->UserName;
		password = KxSecretValue(credentialInfo->CredentialBlob, credentialInfo->CredentialBlobSize);
		
		status = true;
	}

	::CredFree(credentialInfo);
	return status;
}
bool KxSecretStore::Delete(const wxString& serviceName)
{
	return ::CredDeleteW(serviceName.wc_str(), CRED_TYPE_GENERIC, 0);
}
