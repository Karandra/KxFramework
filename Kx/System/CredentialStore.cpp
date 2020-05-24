#include "stdafx.h"
#include "CredentialStore.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include <Windows.h>
#include <wincred.h>
#include "UndefWindows.h"

namespace kxf
{
	bool CredentialStore::Save(const String& userName, const SecretValue& secret)
	{
		if (secret && secret.GetSize() <= CRED_MAX_CREDENTIAL_BLOB_SIZE)
		{
			CREDENTIALW credentialInfo = {0};
			credentialInfo.Type = CRED_TYPE_GENERIC;
			credentialInfo.Persist = CRED_PERSIST_LOCAL_MACHINE;
			credentialInfo.TargetName = const_cast<wchar_t*>(m_ServiceName.wc_str());
			credentialInfo.UserName = const_cast<wchar_t*>(userName.wc_str());
			credentialInfo.CredentialBlob = static_cast<uint8_t*>(const_cast<void*>(secret.GetData()));
			credentialInfo.CredentialBlobSize = secret.GetSize();

			return ::CredWriteW(&credentialInfo, 0);
		}
		return false;
	}
	bool CredentialStore::Load(String& userName, SecretValue& secret) const
	{
		PCREDENTIALW credentialInfo = nullptr;
		Utility::CallAtScopeExit atExit([&]()
		{
			::CredFree(credentialInfo);
		});

		bool status = false;
		if (::CredReadW(m_ServiceName.wc_str(), CRED_TYPE_GENERIC, 0, &credentialInfo))
		{
			userName = credentialInfo->UserName;
			secret = SecretValue(credentialInfo->CredentialBlob, credentialInfo->CredentialBlobSize);
			::RtlSecureZeroMemory(credentialInfo->CredentialBlob, credentialInfo->CredentialBlobSize);

			return true;
		}
		return false;
	}
	bool CredentialStore::Delete()
	{
		return ::CredDeleteW(m_ServiceName.wc_str(), CRED_TYPE_GENERIC, 0);
	}
}
