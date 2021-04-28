#pragma once
#include "Common.h"
#include "../IWebRequestOptions.h"
#include "../IWebRequestAuthOptions.h"
#include "../IWebRequestSecurityOptions.h"
#include "CURLAuthChallenge.h"
#include "CURLUtility.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"

namespace kxf
{
	class CURLSession;
}

namespace kxf
{
	class KX_API CURLRequest final: public RTTI::Implementation<CURLRequest, IWebRequest, IWebRequestOptions, IWebRequestAuthOptions, IWebRequestSecurityOptions>
	{
		friend class CURLSession;
		friend class CURLAuthChallenge;

		private:
			using TCURLOffset = int64_t;

		private:
			static size_t OnReadDataCB(char* data, size_t size, size_t count, void* userData)
			{
				return reinterpret_cast<CURLRequest*>(userData)->OnReadData(data, size, count);
			}
			static size_t OnWriteDataCB(char* data, size_t size, size_t count, void* userData)
			{
				return reinterpret_cast<CURLRequest*>(userData)->OnWriteData(data, size, count);
			}
			static size_t OnReceiveHeaderCB(char* data, size_t size, size_t count, void* userData)
			{
				return reinterpret_cast<CURLRequest*>(userData)->OnReceiveHeader(data, size, count);
			}
			static int OnProgressNotifyCB(void* userData, TCURLOffset bytesExpectedToReceive, TCURLOffset bytesReceived, TCURLOffset bytesExpectedToSend, TCURLOffset bytesSent)
			{
				return reinterpret_cast<CURLRequest*>(userData)->OnProgressNotify(bytesReceived, bytesExpectedToReceive, bytesSent, bytesExpectedToSend);
			}

		private:
			// Control and event handling
			EvtHandler m_EvtHandler;
			CURLSession& m_Session;
			std::weak_ptr<CURLRequest> m_SelfRef;

			std::atomic<WebRequestState> m_State = WebRequestState::None;
			std::atomic<WebRequestState> m_NextState = WebRequestState::None;

			// Request data
			CURL::Private::SessionHandle m_Handle;
			std::vector<WebRequestHeader> m_RequestHeaders;
			void* m_RequestHeadersSList = nullptr;
			String m_Method;
			URI m_URI;

			// Send
			std::shared_ptr<IInputStream> m_SendStream;
			std::string m_SendData;
			WebRequestStorage m_SendStorage = WebRequestStorage::None;

			// Receive
			std::shared_ptr<IOutputStream> m_ReceiveStream;
			WebRequestStorage m_ReceiveStorage = WebRequestStorage::None;

			std::optional<CURLAuthChallenge> m_AuthChallenge;

			// Progress state
			std::atomic<int64_t> m_BytesReceived = -1;
			std::atomic<int64_t> m_BytesExpectedToReceive = -1;
			std::atomic<int64_t> m_BytesSent = -1;
			std::atomic<int64_t> m_BytesExpectedToSend = -1;

		private:
			// CURLRequest
			EventSystem::EvtHandlerAccessor AccessEvtHandler() noexcept
			{
				return m_EvtHandler;
			}

			void WeakRef(const std::shared_ptr<CURLRequest>& selfRef)
			{
				m_SelfRef = selfRef;
			}
			std::shared_ptr<CURLRequest> LockRef() const
			{
				return m_SelfRef.lock();
			}

			void NotifyEvent(EventTag<WebRequestEvent> eventID, WebRequestEvent& event);
			void NotifyStateChange(WebRequestState state, std::optional<int> statusCode = {}, String statusText = {})
			{
				WebRequestEvent event(*this, state, std::move(statusCode), std::move(statusText));
				NotifyEvent(WebRequestEvent::EvtStateChanged, event);
			}

			void DoFreeRequestHeaders();
			void DoSetRequestHeaders();
			void DoPrepareSendData();
			void DoPrepareReceiveData();
			void DoPerformRequest();
			void DoResetState();
			void DoCloseRequest() noexcept;

			bool OnCallbackCommon(bool isWrite, size_t& result);
			size_t OnReadData(char* data, size_t size, size_t count);
			size_t OnWriteData(char* data, size_t size, size_t count);
			size_t OnReceiveHeader(char* data, size_t size, size_t count);
			int OnProgressNotify(int64_t bytesReceived, int64_t bytesExpectedToReceive, int64_t bytesSent, int64_t bytesExpectedToSend);

			void OnUnauthorizedRequest(const HTTPStatus& responseStatus, std::string_view statusText);
			void OnSetAuthChallengeCredentials(WebAuthChallengeSource source, UserCredentials credentials);

		protected:
			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return AccessEvtHandler().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return AccessEvtHandler().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return AccessEvtHandler().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem) override
			{
				return AccessEvtHandler().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				return AccessEvtHandler().OnDynamicUnbind(eventItem);
			}

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return AccessEvtHandler().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return AccessEvtHandler().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return AccessEvtHandler().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return AccessEvtHandler().TryAfter(event);
			}

		public:
			CURLRequest(CURLSession& session, const URI& uri = {});
			CURLRequest(const CURLRequest&) = delete;
			~CURLRequest() noexcept
			{
				DoCloseRequest();
			}

		public:
			// IWebRequest: Common
			bool Start() override;
			bool Pause() override;
			bool Resume() override;
			bool Cancel() override;

			IWebResponse& GetResponse() override
			{
				return NullWebResponse::Get();
			}
			IWebAuthChallenge& GetAuthChallenge() override
			{
				if (m_AuthChallenge)
				{
					return *m_AuthChallenge;
				}
				return NullWebAuthChallenge::Get();
			}
			
			URI GetURI() const override
			{
				return m_URI;
			}
			void* GetNativeHandle() const override
			{
				return m_Handle.GetNativeHandle();
			}

			// IWebRequest: Request options
			bool SetHeader(const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags) override;
			void ClearHeaders() override;

			bool SetSendStorage(WebRequestStorage storage) override;
			bool SetSendSource(std::shared_ptr<IInputStream> stream) override;
			bool SetSendSource(const FSPath& filePath) override;
			bool SetSendSource(const String& data) override;

			bool SetReceiveStorage(WebRequestStorage storage) override;
			bool SetReceiveTarget(std::shared_ptr<IOutputStream> stream) override;
			bool SetReceiveTarget(const FSPath& filePath) override;

			// IWebRequest: Progress
			WebRequestState GetState() const override
			{
				return m_State;
			}

			BinarySize GetBytesSent() const override
			{
				return BinarySize::FromBytes(m_BytesSent);
			}
			BinarySize GetBytesExpectedToSend() const override
			{
				return BinarySize::FromBytes(m_BytesExpectedToSend);
			}
			TransferRate GetSendRate() const override;

			BinarySize GetBytesReceived() const override
			{
				return BinarySize::FromBytes(m_BytesReceived);
			}
			BinarySize GetBytesExpectedToReceive() const override
			{
				return BinarySize::FromBytes(m_BytesExpectedToReceive);
			}
			TransferRate GetReceiveRate() const override;

			// IWebRequestOptions
			bool SetURI(const URI& uri) override;
			bool SetPort(uint16_t port) override;
			bool SetMethod(const String& method) override;
			bool SetDefaultProtocol(const String& protocol) override;
			bool SetAllowedProtocols(FlagSet<WebRequestProtocol> protocols) override;

			bool SetServiceName(const String& name) override;
			bool SetAllowRedirection(WebRequestOption2 option) override;
			bool SetRedirectionProtocols(FlagSet<WebRequestProtocol> protocols) override;
			bool SetResumeOffset(StreamOffset offset) override;

			bool SetRequestTimeout(const TimeSpan& timeout) override;
			bool SetConnectionTimeout(const TimeSpan& timeout) override;

			bool SetMaxSendRate(const TransferRate& rate) override;
			bool SetMaxReceiveRate(const TransferRate& rate) override;

			bool SetKeepAlive(WebRequestOption2 option) override;
			bool SetKeepAliveIdle(const TimeSpan& interval) override;
			bool SetKeepAliveInterval(const TimeSpan& interval) override;

			// IWebRequestAuthOptions
			bool SetAuthMethod(WebRequestAuthMethod method) override;
			bool SetAuthMethods(FlagSet<WebRequestAuthMethod> methods) override;

			bool SetUserName(const String& userName) override;
			bool SetUserPassword(const String& userPassword) override;

			// IWebRequestSecurityOptions
			bool SetUseSSL(WebRequestOption3 option) override;
			bool SetSSLVersion(WebRequestSSLVersion version) override;
			bool SetMaxSSLVersion(WebRequestSSLVersion version) override;

			bool SetVerifyPeer(WebRequestOption2 option) override;
			bool SetVerifyHost(WebRequestOption2 option) override;
			bool SetVerifyStatus(WebRequestOption2 option) override;

		public:
			// IEvtHandler
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler.ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler.DiscardPendingEvents();
			}

			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler.GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler.GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler.SetPrevHandler(evtHandler);
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler.SetNextHandler(evtHandler);
			}

			void Unlink() override
			{
				m_EvtHandler.Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler.IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler.IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler.EnableEventProcessing(enable);
			}

		public:
			Enumerator<String> EnumReplyCookies() const;

		public:
			CURLRequest& operator=(const CURLRequest&) = delete;

			explicit operator bool() const noexcept
			{
				return !m_Handle.IsNull();
			}
			bool operator!() const noexcept
			{
				return m_Handle.IsNull();
			}
	};
}
