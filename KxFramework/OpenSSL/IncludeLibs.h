#pragma comment(lib, "Crypt32.lib")
#if defined _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "KxFramework/OpenSSL/x64/LibSSLMTd.lib")
		#pragma comment(lib, "KxFramework/OpenSSL/x64/LibCryptoMTd.lib")
	#else
		#pragma comment(lib, "KxFramework/OpenSSL/x64/LibSSLMT.lib")
		#pragma comment(lib, "KxFramework/OpenSSL/x64/LibCryptoMT.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "KxFramework/OpenSSL/x86/LibSSLMTd.lib")
		#pragma comment(lib, "KxFramework/OpenSSL/x86/LibCryptoMTd.lib")
	#else
		#pragma comment(lib, "KxFramework/OpenSSL/x86/LibSSLMT.lib")
		#pragma comment(lib, "KxFramework/OpenSSL/x86/LibCryptoMT.lib")
	#endif
#endif
