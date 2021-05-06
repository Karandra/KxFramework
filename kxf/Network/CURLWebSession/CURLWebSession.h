#pragma once
#include "Common.h"
#include "../Private/BasicWebSession.h"
#include "kxf/General/ILibraryInfo.h"

namespace kxf
{
	class KX_API CURLWebSession final: public RTTI::DynamicImplementation<CURLWebSession, Private::BasicWebSession, ILibraryInfo>
	{
		private:
			IFileSystem* m_FileSystem = nullptr;
			std::vector<WebRequestHeader> m_CommonHeaders;

		public:
			CURLWebSession(optional_ptr<IThreadPool> threadPool = nullptr);

		public:
			// IWebSession
			std::shared_ptr<IWebRequest> CreateRequest(const URI& uri) override;

			bool SetCommonHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags) override
			{
				return SetHeader(m_CommonHeaders, header, flags);
			}
			void ClearCommonHeaders() override
			{
				m_CommonHeaders.clear();
			}

			IFileSystem& GetFileSystem() const override;
			void SetFileSystem(IFileSystem& fileSystem) override;

			void* GetNativeHandle() const override
			{
				return nullptr;
			}

			// ILibraryInfo
			String GetName() const override;
			Version GetVersion() const override;
			uint32_t GetAPILevel() const override;

			String GetLicense() const override;
			String GetLicenseName() const override;
			String GetCopyright() const override;
	};
}
