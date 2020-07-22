#pragma once
#include "../Common.h"
#include "../ICoreApplication.h"
#include "kxf/wxWidgets/Application.h"
#include <wx/cmdline.h>

namespace kxf::Application::Private
{
	class KX_API NativeApp final: public wxWidgets::Application
	{
		public:
			static NativeApp* GetInstance()
			{
				return static_cast<NativeApp*>(wxAppConsole::GetInstance());
			}

		private:
			ICoreApplication& m_App;

		private:
			void OnCreate();
			void OnDestroy();

		public:
			NativeApp(ICoreApplication& app)
				:m_App(app)
			{
				OnCreate();
			}
			~NativeApp()
			{
				OnDestroy();
			}

		public:
			// Exceptions support
			bool OnExceptionInMainLoop() override
			{
				return m_App.OnMainLoopException();
			}
			void OnUnhandledException() override
			{
				m_App.OnUnhandledException();
			}
			void OnFatalException() override
			{
				m_App.OnFatalException();
			}
			void OnAssertFailure(const wxChar* file, int line, const wxChar* function, const wxChar* condition, const wxChar* message) override
			{
				m_App.OnAssertFailure(file, line, function, condition, message);
			}

			bool StoreCurrentException() override
			{
				return m_App.StoreCurrentException();
			}
			void RethrowStoredException() override
			{
				m_App.RethrowStoredException();
			}

			// Callbacks for application-wide events
			bool OnInit() override
			{
				return false;
			}
			int OnExit() override
			{
				return -1;
			}
			int OnRun() override
			{
				return m_App.OnRun();
			}

			// Event handling
			int MainLoop() override
			{
				return -1;
			}
			void ExitMainLoop() override
			{
				m_App.ExitMainLoop();
			}
			void OnEventLoopEnter(wxEventLoopBase* loop) override
			{
			}
			void OnEventLoopExit(wxEventLoopBase* loop) override
			{
			}

			bool Pending() override
			{
				return m_App.Pending();
			}
			bool Dispatch() override
			{
				return m_App.Dispatch();
			}
			bool ProcessIdle() override
			{
				return m_App.DispatchIdle();
			}
			void WakeUpIdle() override
			{
				m_App.WakeUp();
			}

			// Pending events
			void ProcessPendingEvents() override
			{
				m_App.ProcessPendingEvents();
				Application::ProcessPendingEvents();
			}
			void DeletePendingObjects()
			{
				Application::DeletePendingObjects();
			}

			// Command line
			void OnInitCmdLine(wxCmdLineParser& parser) override
			{
				m_App.OnCommandLineInit(parser);
			}
			bool OnCmdLineParsed(wxCmdLineParser& parser) override
			{
				return m_App.OnCommandLineParsed(parser);
			}
			bool OnCmdLineError(wxCmdLineParser& parser) override
			{
				return m_App.OnCommandLineError(parser);
			}
			bool OnCmdLineHelp(wxCmdLineParser& parser) override
			{
				return m_App.OnCommandLineHelp(parser);
			}
	};
}
