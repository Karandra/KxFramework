#pragma once
#include "Common.h"
#include "kxf/General/ILibraryInfo.h"
#include "kxf/General/OptionalPtr.h"
#include "kxf/Threading/IThreadPool.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"

namespace kxf
{
	class CURLRequest;
}

namespace kxf
{
	class KX_API CURLSession final: public RTTI::DynamicImplementation<CURLSession, IWebSession, ILibraryInfo>
	{
		friend class CURLRequest;

		private:
			// CURLSession
			static bool SetHeader(std::vector<WebRequestHeader>& headerStorage, const WebRequestHeader& header, FlagSet<WebRequestHeaderFlag> flags);

		private:
			EvtHandler m_EvtHandler;
			optional_ptr<IThreadPool> m_ThreadPool;
			IFileSystem* m_FileSystem = nullptr;

			std::vector<WebRequestHeader> m_CommonHeaders;

		private:
			// CURLSession
			bool IsNull() const noexcept
			{
				return m_ThreadPool.is_null();
			}
			EventSystem::EvtHandlerAccessor AccessEvtHandler() noexcept
			{
				return m_EvtHandler;
			}

			bool StartRequest(CURLRequest& request);

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
			CURLSession(optional_ptr<IThreadPool> threadPool = nullptr);

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

			// ILibraryInfo
			String GetName() const override;
			Version GetVersion() const override;
			uint32_t GetAPILevel() const override;

			String GetLicense() const override;
			String GetLicenseName() const override;
			String GetCopyright() const override;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
