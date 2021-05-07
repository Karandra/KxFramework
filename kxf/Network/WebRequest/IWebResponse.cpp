#include "KxfPCH.h"
#include "IWebResponse.h"
#include "WebRequestHeader.h"
#include "kxf/General/RegEx.h"
#include "kxf/IO/NullStream.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Serialization/JSON.h"
#include "kxf/Serialization/XML.h"

namespace
{
	kxf::NullWebResponse g_NullWebResponse;
}

namespace kxf
{
	FSPath IWebResponse::GetSuggestedFilePath() const
	{
		if (String contentDisposition = GetHeader(wxS("Content-Disposition")); !contentDisposition.IsEmpty())
		{
			if (RegEx regEx(wxS("filename=\"(.+)\""), RegExFlag::IgnoreCase); regEx.Matches(contentDisposition))
			{
				return regEx.GetMatch(contentDisposition, 1);
			}
		}

		if (URI uri = GetURI())
		{
			if (uri.HasPath())
			{
				return uri.GetPath();
			}
			return uri.GetServer();
		}
		return {};
	}

	String IWebResponse::GetAsString() const
	{
		if (auto stream = GetStream())
		{
			IO::InputStreamReader reader(*stream);
			return reader.ReadStringUTF8(stream->GetSize().ToBytes());
		}
		return {};
	}
	JSONDocument IWebResponse::GetAsJSON() const
	{
		if (auto stream = GetStream())
		{
			return *stream;
		}
		return {};
	}
	XMLDocument IWebResponse::GetAsXML() const
	{
		if (auto stream = GetStream())
		{
			return *stream;
		}
		return {};
	}
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
	std::unique_ptr<IInputStream> NullWebResponse::GetStream() const
	{
		return nullptr;
	}
}
