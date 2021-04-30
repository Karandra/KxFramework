#include "KxfPCH.h"
#include "IWebResponse.h"
#include "WebRequestHeader.h"
#include "kxf/IO/NullStream.h"
#include "kxf/FileSystem/FSPath.h"

namespace
{
	kxf::NullWebResponse g_NullWebResponse;
}

namespace kxf
{
	IWebResponse& NullWebResponse::Get()
	{
		return g_NullWebResponse;
	}

	Enumerator<WebRequestHeader> NullWebResponse::EnumHeaders() const
	{
		return {};
	}
	Enumerator<String> NullWebResponse::EnumCookies() const
	{
		return {};
	}

	FSPath NullWebResponse::GetSuggestedFilePath() const
	{
		return {};
	}
	IInputStream& NullWebResponse::GetStream()
	{
		return NullInputStream::Get();
	}
}
