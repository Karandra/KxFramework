#pragma once
#include "KxFramework/KxFramework.h"

class KxCrypto
{
	public:
		enum SHA2Mode
		{
			SHA2_256 = 256,
			SHA2_512 = 512,
		};

	public:
		static const size_t BlockSize = 64 * 1024;
		static const uint32_t CRC32Table[];

	public:
		static wxString GetOpenSSLVersion();

		static wxString Rot13(const wxString& source);
		static wxString CRC32(wxInputStream& stream);
		static wxString MD5(wxInputStream& stream);
		static wxString SHA2(wxInputStream& stream, SHA2Mode keyType);
		static bool Base64Encode(wxInputStream& inputStream, wxOutputStream& outputStream);
		static bool Base64Decode(wxInputStream& inputStream, wxOutputStream& outputStream);
};
