#include "KxfPCH.h"
#include "CURL.h"

#define CURL_STATICLIB 1
#include <curl/curl.h>
#include <curl/urlapi.h>
#include <curl/curlver.h>

namespace kxf::CURL
{
	String GetLibraryName()
	{
		return wxS("libcurl");
	}
	Version GetLibraryVersion()
	{
		return LIBCURL_VERSION;
	}

	String ErrorCodeToString(int code)
	{
		return String::FromUTF8(curl_easy_strerror(static_cast<CURLcode>(code)));
	}
}
