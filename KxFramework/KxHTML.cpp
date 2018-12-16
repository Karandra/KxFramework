/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxHTML.h"
#include "KxFramework/KxUtility.h"

#pragma warning(disable: 4005) // macro redefinition
#include "gumbo.h"
#include "error.h"

#if defined _WIN64
#pragma comment(lib, "KxFramework/HTML/Gumbo x64.lib")
#else
#pragma comment(lib, "KxFramework/HTML/Gumbo x86.lib")
#endif

//////////////////////////////////////////////////////////////////////////
namespace
{
	GumboOptions* GetOptions(void* options)
	{
		return reinterpret_cast<GumboOptions*>(options);
	}
	GumboOutput* GetOutput(void* output)
	{
		return reinterpret_cast<GumboOutput*>(output);
	}
}

void KxHTMLDocument::Init()
{
	GumboOptions* options = new GumboOptions(kGumboDefaultOptions);
	options->userdata = this;

	m_ParserOptions = options;
}
void KxHTMLDocument::Load()
{
	m_ParserOutput = gumbo_parse_with_options(GetOptions(m_ParserOptions), (const char*)m_Buffer.GetData(), m_Buffer.GetDataLen());
	SetNode(GetOutput(m_ParserOutput)->document);
}
void KxHTMLDocument::UnLoad()
{
	if (m_ParserOutput)
	{
		gumbo_destroy_output(GetOptions(m_ParserOptions), GetOutput(m_ParserOutput));
	}

	SetNode(NULL);
	m_Buffer.Clear();
	m_ParserOutput = NULL;
}

const void* KxHTMLDocument::GetNode() const
{
	return GetOutput(m_ParserOutput)->document;
}

KxHTMLDocument::KxHTMLDocument(const wxString& htmlText)
	:KxHTMLNode(NULL, this)
{
	Init();

	if (!htmlText.IsEmpty())
	{
		Load(htmlText);
	}
}
KxHTMLDocument::KxHTMLDocument(wxInputStream& stream)
	:KxHTMLNode(NULL, this)
{
	Init();
	Load(stream);
}
KxHTMLDocument::~KxHTMLDocument()
{
	UnLoad();
	delete GetOptions(m_ParserOptions);
}

bool KxHTMLDocument::Load(const wxString& htmlText)
{
	UnLoad();

	auto utf8 = htmlText.ToUTF8();
	m_Buffer.AppendData(utf8.data(), utf8.length());
	Load();
	return IsOK();
}
bool KxHTMLDocument::Load(wxInputStream& stream)
{
	UnLoad();
	m_Buffer.SetBufSize(stream.GetLength());
	stream.Read(m_Buffer.GetData(), m_Buffer.GetBufSize());
	m_Buffer.SetDataLen(stream.LastRead());

	Load();
	return IsOK();
}
bool KxHTMLDocument::Save(wxOutputStream& stream) const
{
	auto buffer = GetHTML().ToUTF8();
	stream.Write(buffer.data(), buffer.length());
	return stream.IsOk();
}
wxString KxHTMLDocument::Save() const
{
	return GetHTML();
}

bool KxHTMLDocument::IsOK() const
{
	if (!m_Buffer.IsEmpty() && m_ParserOutput)
	{
		const GumboError** pErrors = (const GumboError**)GetOutput(m_ParserOutput)->errors.data;
		if (pErrors)
		{
			for (size_t i = 0; i < GetOutput(m_ParserOutput)->errors.length; i++)
			{
				if (pErrors[i]->type == GUMBO_ERR_PARSER)
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}
