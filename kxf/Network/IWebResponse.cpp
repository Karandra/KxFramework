#include "KxfPCH.h"
#include "IWebResponse.h"
#include "kxf/IO/NullStream.h"

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
	IInputStream& NullWebResponse::GetStream()
	{
		return NullInputStream::Get();
	}
}
