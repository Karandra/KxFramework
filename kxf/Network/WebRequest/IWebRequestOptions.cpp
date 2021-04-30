#include "KxfPCH.h"
#include "IWebRequestOptions.h"

namespace
{
	constexpr struct
	{
		static constexpr kxf::XChar OPTIONS[] = wxS("OPTIONS");
		static constexpr kxf::XChar GET[] = wxS("GET");
		static constexpr kxf::XChar HEAD[] = wxS("HEAD");
		static constexpr kxf::XChar POST[] = wxS("POST");
		static constexpr kxf::XChar PUT[] = wxS("PUT");
		static constexpr kxf::XChar DELETE[] = wxS("DELETE");
		static constexpr kxf::XChar TRACE[] = wxS("TRACE");
		static constexpr kxf::XChar CONNECT[] = wxS("CONNECT");
		static constexpr kxf::XChar PATCH[] = wxS("PATCH");
	} g_MethodNames;
}

namespace kxf
{
	bool IWebRequestOptions::SetMethod(WebRequestMethod method)
	{
		switch (method)
		{
			case WebRequestMethod::None:
			{
				return SetMethod({});
			}
			case WebRequestMethod::OPTIONS:
			{
				return SetMethod(g_MethodNames.OPTIONS);
			}
			case WebRequestMethod::GET:
			{
				return SetMethod(g_MethodNames.GET);
			}
			case WebRequestMethod::HEAD:
			{
				return SetMethod(g_MethodNames.HEAD);
			}
			case WebRequestMethod::POST:
			{
				return SetMethod(g_MethodNames.POST);
			}
			case WebRequestMethod::PUT:
			{
				return SetMethod(g_MethodNames.PUT);
			}
			case WebRequestMethod::DELETE:
			{
				return SetMethod(g_MethodNames.DELETE);
			}
			case WebRequestMethod::TRACE:
			{
				return SetMethod(g_MethodNames.TRACE);
			}
			case WebRequestMethod::CONNECT:
			{
				return SetMethod(g_MethodNames.CONNECT);
			}
			case WebRequestMethod::PATCH:
			{
				return SetMethod(g_MethodNames.PATCH);
			}
		};
		return false;
	}
}
