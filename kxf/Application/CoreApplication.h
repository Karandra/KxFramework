#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include "kxf/Threading/LockGuard.h"
#include "kxf/Threading/RecursiveRWLock.h"

namespace kxf
{
	class KX_API CoreApplication: public RTTI::ImplementInterface<CoreApplication, ICoreApplication>
	{
		public:
			static CoreApplication* GetInstance() noexcept
			{
				return static_cast<CoreApplication*>(ICoreApplication::GetInstance());
			}
			static void SetInstance(CoreApplication* instance) noexcept
			{
				ICoreApplication::SetInstance(instance);
			}

		protected:
			// ICoreApplication
			mutable RecursiveRWLock m_EventFiltersLock;
			std::list<IEventFilter*> m_EventFilters;

			std::optional<int> m_ExitCode;
			bool m_NativeAppInitialized = false;
			bool m_NativeAppCleanedUp = false;

			// Application::IBasicInfo
			String m_Name;
			String m_DisplayName;
			String m_VendorName;
			String m_VendorDisplayName;
			String m_ClassName;
			Version m_Version;

			// Application::IMainEventLoop
			std::unique_ptr<IEventLoop> m_MainLoop;

			// Application::IActiveEventLoop
			IEventLoop* m_ActiveEventLoop = nullptr;

			// Application::IPendingEvents
			std::atomic<bool> m_PendingEventsProcessingEnabled = true;
			
			mutable RecursiveRWLock m_ScheduledForDestructionLock;
			std::vector<std::unique_ptr<wxObject>> m_ScheduledForDestruction;
			
			mutable RecursiveRWLock m_PendingEvtHandlersLock;
			std::list<EvtHandler*> m_PendingEvtHandlers;
			std::list<EvtHandler*> m_DelayedPendingEvtHandlers;

			// Application::IExceptionHandler
			std::exception_ptr m_StoredException;

			// Application::ICommandLine
			size_t m_ArgC = 0;
			char** m_ArgVA = nullptr;
			wchar_t** m_ArgVW = nullptr;

			// CoreApplication
			void* m_DLLNotificationsCookie = nullptr;

		private:
			bool InitDLLNotifications();
			void UninitDLLNotifications();

		protected:
			bool OnDynamicBind(EventItem& eventItem) override;

		public:
			CoreApplication() = default;

		public:
			// IObject
			void* QueryInterface(const IID& iid) noexcept override;

			// ICoreApplication
			bool OnCreate() override;
			void OnDestroy() override;

			bool OnInit() override = 0;
			void OnExit() override;
			int OnRun() override;
			
			void Exit(int exitCode) override;
			std::optional<int> GetExitCode() const override
			{
				return m_ExitCode;
			}

			void AddEventFilter(IEventFilter& eventFilter) override;
			void RemoveEventFilter(IEventFilter& eventFilter) override;
			IEventFilter::Result FilterEvent(IEvent& event) override;

		public:
			// Application::IBasicInfo
			String GetName() const override;
			void SetName(const String& name) override
			{
				m_Name = name;
			}

			String GetDisplayName() const override;
			void SetDisplayName(const String& name) override
			{
				m_DisplayName = name;
			}

			String GetVendorName() const override
			{
				return m_VendorName;
			}
			void SetVendorName(const String& name) override
			{
				m_VendorName = name;
			}

			String GetVendorDisplayName() const override
			{
				return !m_VendorDisplayName.IsEmpty() ? m_VendorDisplayName : m_VendorName;
			}
			void SetVendorDisplayName(const String& name) override
			{
				m_VendorDisplayName = name;
			}

			Version GetVersion() const override
			{
				return m_Version;
			}
			void SetVersion(const Version& version) override
			{
				m_Version = version;
			}

			String GetClassName() const override;
			void SetClassName(const String& name) override;

			// Application::IMainEventLoop
			std::unique_ptr<IEventLoop> CreateMainLoop() override;
			IEventLoop* GetMainLoop() override
			{
				return m_MainLoop.get();
			}
			void ExitMainLoop(int exitCode = 0) override;
			
			void OnEventLoopEnter(IEventLoop& loop) override
			{
				// Nothing to do
			}
			void OnEventLoopExit(IEventLoop& loop) override
			{
				// Nothing to do
			}

			// Application::IActiveEventLoop
			IEventLoop* GetActiveEventLoop() override;
			void SetActiveEventLoop(IEventLoop* eventLoop) override;

			void WakeUp() override;
			bool Pending() override;
			bool Dispatch() override;
			bool DispatchIdle() override;
			bool Yield(FlagSet<EventYieldFlag> flags) override;

			// Application::IPendingEvents
			bool IsPendingEventsProcessingEnabled() const override;
			void EnablePendingEventsProcessing(bool enable = true) override;

			void AddPendingEventHandler(EvtHandler& evtHandler) override;
			bool RemovePendingEventHandler(EvtHandler& evtHandler) override;
			void DelayPendingEventHandler(EvtHandler& evtHandler) override;

			bool ProcessPendingEvents() override;
			size_t DiscardPendingEvents() override;

			bool IsScheduledForDestruction(const wxObject& object) const override;
			void ScheduleForDestruction(std::unique_ptr<wxObject> object) override;
			void FinalizeScheduledForDestruction() override;

			// Application::IExceptionHandler
			bool OnMainLoopException() override;
			void OnFatalException() override;
			void OnUnhandledException() override;

			bool StoreCurrentException() override;
			void RethrowStoredException() override;

			// Application::IDebugHandler
			void OnAssertFailure(String file, int line, String function, String condition, String message) override;

			// Application::ICommandLine
			void InitializeCommandLine(char** argv, size_t argc) override
			{
				m_ArgC = argc;
				m_ArgVA = argv;
				m_ArgVW = nullptr;
			}
			void InitializeCommandLine(wchar_t** argv, size_t argc) override
			{
				m_ArgC = argc;
				m_ArgVA = nullptr;
				m_ArgVW = argv;
			}

			size_t EnumCommandLineArgs(std::function<bool(String)> func) const override;
			void OnCommandLineInit(wxCmdLineParser& parser) override;
			bool OnCommandLineParsed(wxCmdLineParser& parser) override;
			bool OnCommandLineError(wxCmdLineParser& parser) override;
			bool OnCommandLineHelp(wxCmdLineParser& parser) override;
	};
}
