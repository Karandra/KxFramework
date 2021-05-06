#include "KxfPCH.h"
#include "CURLSession.h"
#include "CURLRequest.h"
#include "kxf/FileSystem/IFileSystem.h"
#include <curl/curlver.h>

namespace
{
	constexpr char g_License[] = R"~~~(COPYRIGHT AND PERMISSION NOTICE

Copyright (c) 1996 - 2021, Daniel Stenberg, daniel@haxx.se, and many contributors, see the THANKS file.

All rights reserved.

Permission to use, copy, modify, and distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.)~~~";
}

namespace kxf
{
	// IWebSession
	std::shared_ptr<IWebRequest> CURLSession::CreateRequest(const URI& uri)
	{
		auto request = std::make_shared<CURLRequest>(*this, m_CommonHeaders, uri);
		if (!request->IsNull())
		{
			request->WeakRef(request);
			return request;
		}
		return nullptr;
	}

	IFileSystem& CURLSession::GetFileSystem() const
	{
		return m_FileSystem ? *m_FileSystem : FileSystem::GetNullFileSystem();
	}
	void kxf::CURLSession::SetFileSystem(IFileSystem& fileSystem)
	{
		if (fileSystem && fileSystem.IsLookupScoped())
		{
			m_FileSystem = &fileSystem;
		}
		else
		{
			m_FileSystem = nullptr;
		}
	}

	// ILibraryInfo
	String CURLSession::GetName() const
	{
		return wxS("libcurl");
	}
	Version CURLSession::GetVersion() const
	{
		return LIBCURL_VERSION;
	}
	uint32_t CURLSession::GetAPILevel() const
	{
		return LIBCURL_VERSION_NUM;
	}

	String CURLSession::GetLicense() const
	{
		return g_License;
	}
	String CURLSession::GetLicenseName() const
	{
		return wxS("MIT/X inspired");
	}
	String CURLSession::GetCopyright() const
	{
		return LIBCURL_COPYRIGHT;
	}
}
