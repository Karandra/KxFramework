#pragma once
#include "KxFramework/KxFramework.h"

namespace KxCrypto
{
	enum SHA2Mode
	{
		SHA2_256 = 256,
		SHA2_512 = 512,
	};
}

namespace KxCrypto
{
	KX_API wxString GetOpenSSLVersion();

	KX_API wxString Rot13(const wxString& source);
	KX_API wxString CRC32(wxInputStream& stream);
	KX_API wxString MD5(wxInputStream& stream);
	KX_API wxString SHA2(wxInputStream& stream, SHA2Mode keyType);
	KX_API bool Base64Encode(wxInputStream& inputStream, wxOutputStream& outputStream);
	KX_API bool Base64Decode(wxInputStream& inputStream, wxOutputStream& outputStream);
};
