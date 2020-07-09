#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/IEventFilter.h"
#include "Private/NativeApp.h"

namespace kxf
{
	class KX_API CoreApplication: public RTTI::ImplementInterface<CoreApplication, ICoreApplication, IEventFilter>
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
			Version m_Version;

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

			// CoreApplication
			wxEventLoopBase* GetMainLoop() const
			{
				return m_NativeApp.wxApp::GetMainLoop();
			}
			bool Yield()
			{
				return m_NativeApp.wxApp::Yield(true);
			}
			
		public:
			// ICoreApplication
			void Exit(int exitCode) override
			{
				m_NativeApp.Exit(exitCode);
			}
			std::optional<int> GetExitCode() const override
			{
				return m_NativeApp.GetExitCode();
			}

			// Application information
			String GetName() const override
			{
				return m_NativeApp.GetAppName();
			}
			void SetName(const String& name) override
			{
				m_NativeApp.SetAppName(name);
			}

			String GetDisplayName() const override
			{
				return m_NativeApp.GetAppDisplayName();
			}
			void SetDisplayName(const String& name) override
			{
				m_NativeApp.SetAppDisplayName(name);
			}

			String GetVendorName() const override
			{
				return m_NativeApp.GetVendorName();
			}
			void SetVendorName(const String& name) override
			{
				m_NativeApp.SetVendorName(name);
			}

			String GetVendorDisplayName() const override
			{
				return m_NativeApp.GetVendorDisplayName();
			}
			void SetVendorDisplayName(const String& name) override
			{
				m_NativeApp.SetVendorDisplayName(name);
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

			// Callbacks for application-wide events
			bool OnInit() override = 0;
			void OnExit() override
			{
				static_cast<void>(m_NativeApp.wxApp::OnExit());
			}
			int OnRun() override
			{
				return m_NativeApp.wxApp::OnRun();
			}

			// Event handling
			int MainLoop() override
			{
				return m_NativeApp.wxApp::MainLoop();
			}
			void ExitMainLoop() override
			{
				m_NativeApp.wxApp::ExitMainLoop();
			}
			void OnEventLoopEnter(IEventLoop& loop) override
			{
				//m_NativeApp.wxApp::OnEventLoopEnter(&loop);
			}
			void OnEventLoopExit(IEventLoop& loop) override
			{
				//m_NativeApp.wxApp::OnEventLoopExit(&loop);
			}
			bool DispatchIdle() override;
			
			void ExecuteEventHandler(Event& event, IEventExecutor& executor, EvtHandler& evtHandler) override;

			// Pending events
			bool ProcessPendingEvents() override
			{
				m_NativeApp.wxApp::ProcessPendingEvents();
				return false;
			}
			size_t DiscardPendingEvents() override
			{
				m_NativeApp.wxApp::DeletePendingEvents();
				return 0;
			}

			void SuspendPendingEventsProcessing() override
			{
				m_NativeApp.wxApp::SuspendProcessingOfPendingEvents();
			}
			void ResumePendingEventsProcessing() override
			{
				m_NativeApp.wxApp::ResumeProcessingOfPendingEvents();
			}

			bool IsScheduledForDestruction(const wxObject& object) const override
			{
				return m_NativeApp.wxApp::IsScheduledForDestruction(const_cast<wxObject*>(&object));
			}
			void ScheduledForDestruction(wxObject& object) override
			{
				return m_NativeApp.wxApp::ScheduleForDestruction(&object);
			}
			void FinalizeScheduledForDestruction() override
			{
				m_NativeApp.wxApp::DeletePendingEvents();
			}

			// Exceptions support
			bool OnMainLoopException() override
			{
				return m_NativeApp.wxApp::OnExceptionInMainLoop();
			}
			void OnFatalException() override
			{
				m_NativeApp.wxApp::OnFatalException();
			}
			void OnUnhandledException() override
			{
				m_NativeApp.wxApp::OnUnhandledException();
			}
			void OnAssertFailure(String file, int line, String function, String condition, String message) override
			{
				m_NativeApp.wxApp::OnAssertFailure(file.wx_str(), line, function.wx_str(), condition.wx_str(), message.wx_str());
			}

			bool StoreCurrentException() override
			{
				return m_NativeApp.wxApp::StoreCurrentException();
			}
			void RethrowStoredException() override
			{
				return m_NativeApp.wxApp::RethrowStoredException();
			}

			// Command line
			size_t EnumCommandLineArgs(std::function<bool(String)> func) const override;
			void OnCommandLineInit(wxCmdLineParser& parser) override
			{
				m_NativeApp.wxApp::OnInitCmdLine(parser);
			}
			bool OnCommandLineParsed(wxCmdLineParser& parser) override
			{
				return m_NativeApp.wxApp::OnCmdLineParsed(parser);
			}
			bool OnCommandLineError(wxCmdLineParser& parser) override
			{
				return m_NativeApp.wxApp::OnCmdLineError(parser);
			}
			bool OnCommandLineHelp(wxCmdLineParser& parser) override
			{
				return m_NativeApp.wxApp::OnCmdLineHelp(parser);
			}

		public:
			// IEventFilter
			IEventFilter::Result FilterEvent(Event& event) override
			{
				return IEventFilter::Result::Skip;
			}
	};
}
