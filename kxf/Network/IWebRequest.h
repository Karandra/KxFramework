#pragma once
#include "Common.h"
#include "URI.h"
#include "WebRequestHeader.h"
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

	class FSPath;
	class IInputStream;
	class IOutputStream;

	enum class WebRequestState
	{
		None = -1,

		Idle,
		Paused,
		Resumed,
		Unauthorized,
		Active,
		Completed,
		Failed,
		Cancelled
	};
	enum class WebRequestStorage
	{
		None = -1,

		Memory,
		Stream,
		FileSystem
	};
}

namespace kxf
{
	class IWebRequest: public RTTI::ExtendInterface<IWebRequest, IEvtHandler>
	{
		KxRTTI_DeclareIID(IWebRequest, {0xd3a87d4e, 0xf22f, 0x45e1, {0x8c, 0xef, 0x4c, 0x79, 0xa0, 0xf, 0xb, 0x30}});

		public:
			// Common
			virtual bool Start() = 0;
			virtual bool Pause() = 0;
			virtual bool Resume() = 0;
			virtual bool Cancel() = 0;

			virtual IWebResponse& GetResponse() = 0;
			virtual IWebAuthChallenge& GetAuthChallenge() = 0;

			virtual URI GetURI() const = 0;
			virtual void* GetNativeHandle() const = 0;

			// Request options
			virtual bool SetHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags) = 0;
			virtual void ClearHeaders() = 0;

			virtual bool SetSendStorage(WebRequestStorage storage) = 0;
			virtual bool SetSendSource(std::shared_ptr<IInputStream> stream) = 0;
			virtual bool SetSendSource(const FSPath& filePath) = 0;
			virtual bool SetSendSource(const String& data) = 0;

			virtual bool SetReceiveStorage(WebRequestStorage storage) = 0;
			virtual bool SetReceiveTarget(std::shared_ptr<IOutputStream> stream) = 0;
			virtual bool SetReceiveTarget(const FSPath& filePath) = 0;

			// Progress
			virtual WebRequestState GetState() const = 0;

			virtual BinarySize GetBytesSent() const = 0;
			virtual BinarySize GetBytesExpectedToSend() const = 0;
			virtual TransferRate GetSendRate() const = 0;

			virtual BinarySize GetBytesReceived() const = 0;
			virtual BinarySize GetBytesExpectedToReceive() const = 0;
			virtual TransferRate GetReceiveRate() const = 0;
	};
}
