#include "KxfPCH.h"
#include "IWebRequest.h"

namespace
{
	constexpr kxf::XChar g_UserAgent[] = wxS("User-Agent");

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
	void IWebRequest::SetUserAgent(const String& value)
	{
		SetHeader(g_UserAgent, value);
	}

	WebRequestCommonMethod IWebRequest::GetCommonMethod() const
	{
		const String method = GetMethod();

		if (method.IsEmpty())
		{
			return WebRequestCommonMethod::None;
		}
		if (method == g_MethodNames.OPTIONS)
		{
			return WebRequestCommonMethod::OPTIONS;
		}
		else if (method == g_MethodNames.GET)
		{
			return WebRequestCommonMethod::GET;
		}
		else if (method == g_MethodNames.HEAD)
		{
			return WebRequestCommonMethod::HEAD;
		}
		else if (method == g_MethodNames.POST)
		{
			return WebRequestCommonMethod::POST;
		}
		else if (method == g_MethodNames.PUT)
		{
			return WebRequestCommonMethod::PUT;
		}
		else if (method == g_MethodNames.DELETE)
		{
			return WebRequestCommonMethod::DELETE;
		}
		else if (method == g_MethodNames.TRACE)
		{
			return WebRequestCommonMethod::TRACE;
		}
		else if (method == g_MethodNames.PATCH)
		{
			return WebRequestCommonMethod::PATCH;
		}
		else if (method == g_MethodNames.CONNECT)
		{
			return WebRequestCommonMethod::CONNECT;
		}
		return WebRequestCommonMethod::Unknown;
	}
	void IWebRequest::SetCommonMethod(WebRequestCommonMethod method)
	{
		switch (method)
		{
			case WebRequestCommonMethod::None:
			{
				SetMethod(NullString);
				break;
			}
			case WebRequestCommonMethod::OPTIONS:
			{
				SetMethod(g_MethodNames.OPTIONS);
				break;
			}
			case WebRequestCommonMethod::GET:
			{
				SetMethod(g_MethodNames.GET);
				break;
			}
			case WebRequestCommonMethod::HEAD:
			{
				SetMethod(g_MethodNames.HEAD);
				break;
			}
			case WebRequestCommonMethod::POST:
			{
				SetMethod(g_MethodNames.POST);
				break;
			}
			case WebRequestCommonMethod::PUT:
			{
				SetMethod(g_MethodNames.PUT);
				break;
			}
			case WebRequestCommonMethod::DELETE:
			{
				SetMethod(g_MethodNames.DELETE);
				break;
			}
			case WebRequestCommonMethod::TRACE:
			{
				SetMethod(g_MethodNames.TRACE);
				break;
			}
			case WebRequestCommonMethod::CONNECT:
			{
				SetMethod(g_MethodNames.CONNECT);
				break;
			}
			case WebRequestCommonMethod::PATCH:
			{
				SetMethod(g_MethodNames.PATCH);
				break;
			}
		};
	}
}
