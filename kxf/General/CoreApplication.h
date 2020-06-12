#pragma once
#include "Common.h"
#include "ICoreApplication.h"
#include "Private/AppImpl.h"

namespace kxf
{
	class KX_API CoreApplication: public RTTI::ImplementInterface<CoreApplication, ICoreApplication>
	{
		public:
			static bool IsMainLoopRunning() noexcept
			{
				return wxApp::IsMainLoopRunning();
			}

		private:
			Private::AppImpl m_App;
			Version m_Version;

		protected:
			Private::AppImpl& GetImpl()
			{
				return m_App;
			}
			const Private::AppImpl& GetImpl() const
			{
				return m_App;
			}

		public:
			CoreApplication()
				:m_App(*this)
			{
			}

		public:
			void Exit(int exitCode) override
			{
				m_App.Exit(exitCode);
			}
			std::optional<int> GetExitCode() const override
			{
				return m_App.GetExitCode();
			}
			wxApp& GetWxImpl() override
			{
				return m_App;
			}

			// Application information
			String GetName() const override
			{
				return m_App.GetAppName();
			}
			void SetName(const String& name) override
			{
				m_App.SetAppName(name);
			}

			String GetDisplayName() const override
			{
				return m_App.GetAppDisplayName();
			}
			void SetDisplayName(const String& name) override
			{
				m_App.SetAppDisplayName(name);
			}

			String GetVendorName() const override
			{
				return m_App.GetVendorName();
			}
			void SetVendorName(const String& name) override
			{
				m_App.SetVendorName(name);
			}

			String GetVendorDisplayName() const override
			{
				return m_App.GetVendorDisplayName();
			}
			void SetVendorDisplayName(const String& name) override
			{
				m_App.SetVendorDisplayName(name);
			}

			String GetClassName() const override
			{
				return m_App.GetClassName();
			}
			void SetClassName(const String& name) override
			{
				m_App.SetClassName(name);
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
				m_App.wxApp::OnExit();
			}
			int OnRun() override
			{
				return m_App.wxApp::OnRun();
			}

			// Event handling
			int MainLoop() override
			{
				return m_App.wxApp::MainLoop();
			}
			void ExitMainLoop() override
			{
				m_App.wxApp::ExitMainLoop();
			}
			int FilterEvent(wxEvent& event) override
			{
				return m_App.wxApp::FilterEvent(event);
			}
			wxEventLoopBase* GetMainLoop() const override
			{
				return m_App.wxApp::GetMainLoop();
			}
			void HandleEvent(wxEvtHandler& handler, wxEventFunction func, wxEvent& event) const override
			{
				return m_App.wxApp::HandleEvent(&handler, func, event);
			}
			void OnEventLoopEnter(wxEventLoopBase& loop) override
			{
				m_App.wxApp::OnEventLoopEnter(&loop);
			}
			void OnEventLoopExit(wxEventLoopBase& loop) override
			{
				m_App.wxApp::OnEventLoopExit(&loop);
			}
			wxEvtHandler& GetEvtHandler() override
			{
				return m_App;
			}

			// Pending events
			void ProcessPendingEvents() override
			{
				m_App.wxApp::ProcessPendingEvents();
			}
			void DiscardPendingEvents() override
			{
				m_App.wxApp::DeletePendingEvents();
			}
			bool Yield() override
			{
				return m_App.wxApp::Yield(true);
			}

			void SuspendPendingEventsProcessing() override
			{
				m_App.wxApp::SuspendProcessingOfPendingEvents();
			}
			void ResumePendingEventsProcessing() override
			{
				m_App.wxApp::ResumeProcessingOfPendingEvents();
			}

			bool IsScheduledForDestruction(const wxObject& object) const override
			{
				return m_App.wxApp::IsScheduledForDestruction(const_cast<wxObject*>(&object));
			}
			void ScheduleForDestruction(wxObject& object) override
			{
				return m_App.wxApp::ScheduleForDestruction(&object);
			}

			// Exceptions support
			bool OnMainLoopException() override
			{
				return m_App.wxApp::OnExceptionInMainLoop();
			}
			void OnFatalException() override
			{
				m_App.wxApp::OnFatalException();
			}
			void OnUnhandledException() override
			{
				m_App.wxApp::OnUnhandledException();
			}
			void OnAssertFailure(String file, int line, String function, String condition, String message) override
			{
				m_App.wxApp::OnAssertFailure(file.wx_str(), line, function.wx_str(), condition.wx_str(), message.wx_str());
			}

			bool StoreCurrentException() override
			{
				return m_App.wxApp::StoreCurrentException();
			}
			void RethrowStoredException() override
			{
				return m_App.wxApp::RethrowStoredException();
			}

			// Command line
			size_t EnumCommandLineArgs(std::function<bool(String)> func) override
			{
				size_t count = 0;
				for (size_t i = 0; i < static_cast<size_t>(m_App.argc); i++)
				{
					count++;
					if (!std::invoke(func, m_App.argv[i]))
					{
						break;
					}
				}
				return count;
			}
			void OnCommandLineInit(wxCmdLineParser& parser) override
			{
				m_App.wxApp::OnInitCmdLine(parser);
			}
			bool OnCommandLineParsed(wxCmdLineParser& parser) override
			{
				return m_App.wxApp::OnCmdLineParsed(parser);
			}
			bool OnCommandLineError(wxCmdLineParser& parser) override
			{
				return m_App.wxApp::OnCmdLineError(parser);
			}
			bool OnCommandLineHelp(wxCmdLineParser& parser) override
			{
				return m_App.wxApp::OnCmdLineHelp(parser);
			}
	};
}
