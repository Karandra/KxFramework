#pragma once
#include "Common.h"
#include "WSPP.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/MemoryStream.h"

namespace kxf
{
	class WSPPWebRequest;
}

namespace kxf
{
	class WSPPWebResponse final: public IWebResponse, public Private::WSPPTypes
	{
		friend class WSPPWebRequest;

		private:
			WSPPWebRequest& m_Request;

			String m_StatusText;
			std::optional<int> m_StatusCode;

		private:
			TClient& GetClient() const noexcept;
			TConnection* GetConnection() const noexcept;

		public:
			WSPPWebResponse(WSPPWebRequest& request, std::optional<int> statusCode = {}, String statusText = {})
				:m_Request(request), m_StatusText(std::move(statusText)), m_StatusCode(std::move(statusCode))
			{
			}

		public:
			URI GetURI() const override;
			String GetMethod() const override;
			String GetPrimaryIP() const override;
			std::optional<uint16_t> GetPrimaryPort() const override;
			WebRequestProtocol GetProtocol() const override;
			WebRequestHTTPVersion GetHTTPVersion() const override;
			DataSize GetContentLength() const override;
			String GetContentType() const override;

			std::optional<int> GetStatusCode() const override
			{
				return m_StatusCode;
			}
			String GetStatusText() const override
			{
				return m_StatusText;
			}

			String GetHeader(const String& name) const override;
			Enumerator<WebRequestHeader> EnumHeaders() const override;
			Enumerator<String> EnumCookies() const override;

			std::unique_ptr<IInputStream> GetStream() const override;
	};
}
