#include "stdafx.h"
#include "HTMLDocument.h"

#pragma warning(disable: 4005) // macro redefinition
#include "gumbo.h"
#include "Private/HTML/error.h"

namespace
{
	GumboOptions* GetOptions(void* options) noexcept
	{
		return reinterpret_cast<GumboOptions*>(options);
	}
	GumboOutput* GetOutput(void* output) noexcept
	{
		return reinterpret_cast<GumboOutput*>(output);
	}
}

namespace KxFramework
{
	void KxHTMLDocument::Init()
	{
		auto options = std::make_unique<GumboOptions>(kGumboDefaultOptions);
		options->userdata = this;

		m_ParserOptions = options.release();
	}
	void KxHTMLDocument::DoLoad()
	{
		m_ParserOutput = gumbo_parse_with_options(GetOptions(m_ParserOptions), reinterpret_cast<const char*>(m_Buffer.data()), m_Buffer.size());
		SetNode(GetOutput(m_ParserOutput)->document);
	}
	void KxHTMLDocument::DoUnload()
	{
		if (m_ParserOutput)
		{
			gumbo_destroy_output(GetOptions(m_ParserOptions), GetOutput(m_ParserOutput));
		}

		SetNode(nullptr);
		m_Buffer.clear();
		m_ParserOutput = nullptr;
	}
	void KxHTMLDocument::Destroy()
	{
		DoUnload();

		delete GetOptions(m_ParserOptions);
		m_ParserOptions = nullptr;
	}

	const void* KxHTMLDocument::GetNode() const
	{
		return GetOutput(m_ParserOutput)->document;
	}
	void KxHTMLDocument::SetNode(void* node)
	{
		// Nothing to do
		//GetOutput(m_ParserOutput)->document = reinterpret_cast<GumboNode*>(node);
	}

	bool KxHTMLDocument::Load(const String& htmlText)
	{
		DoUnload();

		auto utf8 = htmlText.ToUTF8();
		m_Buffer.resize(utf8.length());
		std::memcpy(m_Buffer.data(), utf8.data(), utf8.length());

		DoLoad();
		return !IsNull();
	}
	bool KxHTMLDocument::Load(wxInputStream& stream)
	{
		DoUnload();
		m_Buffer.resize(stream.GetLength());
		stream.Read(m_Buffer.data(), m_Buffer.size());
		m_Buffer.resize(stream.LastRead());

		DoLoad();
		return !IsNull();
	}
	bool KxHTMLDocument::Save(wxOutputStream& stream) const
	{
		auto utf8 = GetHTML().ToUTF8();
		return stream.WriteAll(utf8.data(), utf8.length());
	}

	bool KxHTMLDocument::IsNull() const
	{
		if (!m_Buffer.empty() && m_ParserOutput)
		{
			if (GumboError** errors = reinterpret_cast<GumboError**>(GetOutput(m_ParserOutput)->errors.data))
			{
				for (size_t i = 0; i < GetOutput(m_ParserOutput)->errors.length; i++)
				{
					if (errors[i]->type == GUMBO_ERR_PARSER)
					{
						return true;
					}
				}
			}
			return false;
		}
		return true;
	}
}
