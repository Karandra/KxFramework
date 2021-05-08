#include "KxfPCH.h"
#include "WSPPWebSession.h"
#include "WSPPWebRequest.h"
#include "WSPP.h"
#include "kxf/FileSystem/IFileSystem.h"
#include "kxf/Utility/SoftwareLicenseDB.h"

namespace
{
	constexpr kxf::XChar g_Copyright[] = wxS("Copyright (c) 2014, Peter Thorson. All rights reserved.");
}

namespace kxf
{
	WSPPWebSession::WSPPWebSession(optional_ptr<IThreadPool> threadPool)
	{
		BasicWebSession::DoInitialize(std::move(threadPool));
	}

	// IWebSession
	std::shared_ptr<IWebRequest> WSPPWebSession::CreateRequest(const URI& uri)
	{
		auto request = std::make_shared<WSPPWebRequest>(*this, m_CommonHeaders, uri);
		if (!request->IsNull())
		{
			request->WeakRef(request);
			return request;
		}
		return nullptr;
	}

	IFileSystem& WSPPWebSession::GetFileSystem() const
	{
		return m_FileSystem ? *m_FileSystem : FileSystem::GetNullFileSystem();
	}
	void kxf::WSPPWebSession::SetFileSystem(IFileSystem& fileSystem)
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
	String WSPPWebSession::GetName() const
	{
		return wxS("WebSocket++");
	}
	Version WSPPWebSession::GetVersion() const
	{
		return {websocketpp::major_version, websocketpp::minor_version, websocketpp::patch_version};
	}
	uint32_t WSPPWebSession::GetAPILevel() const
	{
		return websocketpp::major_version * 1000 + websocketpp::minor_version * 100 + websocketpp::patch_version * 10;
	}

	String WSPPWebSession::GetLicense() const
	{
		return SoftwareLicenseDB::Get().GetText(SoftwareLicenseType::BSD3_Clause, g_Copyright);
	}
	String WSPPWebSession::GetLicenseName() const
	{
		return SoftwareLicenseDB::Get().GetName(SoftwareLicenseType::BSD3_Clause);
	}
	String WSPPWebSession::GetCopyright() const
	{
		return g_Copyright;
	}
}
