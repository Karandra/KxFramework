#pragma once
#include "Common.h"
#include "URI.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/General/BinarySize.h"
#include "kxf/EventSystem/IEvtHandler.h"

// Undef these symbols because Windows SDK defines some of them
#undef OPTIONS
#undef GET
#undef HEAD
#undef POST
#undef PUT
#undef DELETE
#undef TRACE
#undef CONNECT

namespace kxf
{
	class IWebSession;
	class IWebResponse;
	class IWebAuthChallenge;
	class WebRequestEvent;

	enum class WebRequestState
	{
		None = -1,

		Idle,
		Unauthorized,
		Active,
		Completed,
		Failed,
		Cancelled
	};
	enum class WebRequestPeerVerify
	{
		Disabled = 0,
		Enabled = 1
	};
	enum class WebRequestCommonMethod
	{
		Unknown = -2,
		None = -1,

		OPTIONS,
		GET,
		HEAD,
		POST,
		PUT,
		DELETE,
		TRACE,
		CONNECT,
		PATCH
	};
}

namespace kxf
{
	class IWebRequest: public RTTI::ExtendInterface<IWebRequest, IEvtHandler>
	{
		KxRTTI_DeclareIID(IWebRequest, {0xd3a87d4e, 0xf22f, 0x45e1, {0x8c, 0xef, 0x4c, 0x79, 0xa0, 0xf, 0xb, 0x30}});

		public:
			// Common
			virtual void Start() = 0;
			virtual void Pause() = 0;
			virtual void Resume() = 0;
			virtual void Cancel() = 0;

			virtual IWebResponse& GetResponse() = 0;
			virtual IWebAuthChallenge& GetAuthChallenge() = 0;
			
			virtual URI GetURI() const = 0;
			virtual void* GetNativeHandle() const = 0;

			// Request options
			virtual void SetHeader(const String& name, const String& value) = 0;
			virtual void ClearHeaders() = 0;
			void SetUserAgent(const String& value);

			virtual String GetMethod() const = 0;
			virtual void SetMethod(const String& method) = 0;
			WebRequestCommonMethod GetCommonMethod() const;
			void SetCommonMethod(WebRequestCommonMethod method);

			virtual WebRequestPeerVerify GetPeerVerify() const = 0;
			virtual void SetPeerVerify(WebRequestPeerVerify option) = 0;

			// Progress
			virtual WebRequestState GetState() const = 0;

			virtual BinarySize GetBytesSent() const = 0;
			virtual BinarySize GetBytesExpectedToSend() const = 0;

			virtual BinarySize GetBytesReceived() const = 0;
			virtual BinarySize GetBytesExpectedToReceive() const = 0;
	};
}
