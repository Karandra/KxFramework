#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include "kxf/Threading/LockGuard.h"
#include "kxf/Threading/ReadWriteLock.h"
#include "Private/NativeApp.h"

namespace kxf
{
	class KX_API CoreApplication: public RTTI::ImplementInterface<CoreApplication, ICoreApplication>
	{
		public:
			static bool IsMainLoopRunning() noexcept
			{
				return wxApp::IsMainLoopRunning();
			}

			static CoreApplication* GetInstance() noexcept
			{
				return static_cast<CoreApplication*>(ICoreApplication::GetInstance());
			}
			static void SetInstance(CoreApplication* instance) noexcept
			{
				ICoreApplication::SetInstance(instance);
			}

		private:
			Private::NativeApp m_NativeApp;

			// ICoreApplication
			std::optional<int> m_ExitCode;

			// Application::IBasicInfo
			String m_Name;
			String m_DisplayName;
			String m_VendorName;
			String m_VendorDisplayName;
			String m_ClassName;
			Version m_Version;

			// Application::IMainEventLoop
			std::unique_ptr<IEventLoop> m_MainLoop;

			// Application::IPendingEvents
			std::atomic<bool> m_PendingEventsProcessingEnabled = true;
			
			ReadWriteLock m_ScheduledForDestructionLock;
			std::vector<std::unique_ptr<wxObject>> m_ScheduledForDestruction;
			
			ReadWriteLock m_PendingEvtHandlersLock;
			std::list<EvtHandler*> m_PendingEvtHandlers;
			std::list<EvtHandler*> m_DelayedPendingEvtHandlers;

			// Application::IExceptionHandler
			std::exception_ptr m_StoredException;

			// Application::ICommandLine
			size_t m_ArgC = 0;
			const char** m_ArgVA = nullptr;
			const wchar_t** m_ArgVW = nullptr;

		protected:
			Private::NativeApp& GetImpl()
			{
				return m_NativeApp;
			}
			const Private::NativeApp& GetImpl() const
			{
				return m_NativeApp;
			}

		public:
			CoreApplication()
				:m_NativeApp(*this)
			{
			}

		public:
			// IObject
			void* QueryInterface(const IID& iid) noexcept override
			{
				if (iid.IsOfType<wxWidgets::Application>())
				{
					return static_cast<wxWidgets::Application*>(&m_NativeApp);
				}
				return TBaseClass::QueryInterface(iid);
			}

			// ICoreApplication
			bool OnInit() override = 0;
			void OnExit() override;
			int OnRun() override;
			
			void Exit(int exitCode) override;
			std::optional<int> GetExitCode() const override
			{
				return m_ExitCode;
			}

			// IEventFilter
			IEventFilter::Result FilterEvent(Event& event) override
			{
				return Result::Skip;
			}

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

			String GetClassName() const override
			{
				return m_NativeApp.GetClassName();
			}
			void SetClassName(const String& name) override
			{
				m_NativeApp.SetClassName(name);
			}

			Version GetVersion() const override
			{
				return m_Version;
			}
			void SetVersion(const Version& version) override
			{
				m_Version = version;
			}

			// Application::IMainEventLoop
			int MainLoop() override;
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
			void WakeUp() override;
			bool Pending() override;
			bool Dispatch() override;
			bool DispatchIdle() override;
			bool Yield(FlagSet<EventYieldFlag> flags) override;

			// Application::IPendingEvents
			bool IsPendingEventsProcessingEnabled() const override
			{
				return m_PendingEventsProcessingEnabled;
			}
			void EnablePendingEventsProcessing(bool enable = true) override
			{
				m_PendingEventsProcessingEnabled = enable;
			}

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
				m_ArgVA = argv;
				m_ArgVW = nullptr;
				m_ArgC = argc;
			}
			void InitializeCommandLine(wchar_t** argv, size_t argc) override
			{
				m_ArgVA = nullptr;
				m_ArgVW = argv;
				m_ArgC = argc;
			}

			size_t EnumCommandLineArgs(std::function<bool(String)> func) const override;
			void OnCommandLineInit(wxCmdLineParser& parser) override;
			bool OnCommandLineParsed(wxCmdLineParser& parser) override;
			bool OnCommandLineError(wxCmdLineParser& parser) override;
			bool OnCommandLineHelp(wxCmdLineParser& parser) override;
	};
}
