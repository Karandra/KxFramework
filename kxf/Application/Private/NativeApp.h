#pragma once
#include "../Common.h"
#include "../ICoreApplication.h"
#include "kxf/wxWidgets/Application.h"
#include <wx/cmdline.h>

namespace kxf::Private
{
	class KX_API NativeApp final: public wxWidgets::Application
	{
		private:
			ICoreApplication& m_App;
			void* m_DLLNotificationsCookie = nullptr;

		private:
			void OnCreate();
			void OnDestroy();

			bool OnBindDLLNotification();

		protected:
			bool OnDynamicBind(wxDynamicEventTableEntry& entry) override;

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

			// Callbacks for application-wide events
			bool OnInit() override
			{
				return m_App.OnInit();
			}
			int OnExit() override
			{
				m_App.OnExit();
				return m_App.GetExitCode().value_or(-1);
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

			// Pending events
			void ProcessPendingEvents() override
			{
				m_App.ProcessPendingEvents();
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
