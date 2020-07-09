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
			std::optional<int> m_ExitCode;
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
			void Exit(int exitCode)
			{
				m_ExitCode = exitCode;
				wxExit();
			}
			std::optional<int> GetExitCode() const
			{
				return m_ExitCode;
			}

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

				// Return code from this function is ignored but we can still return something
				return m_ExitCode.value_or(std::numeric_limits<int>::max());
			}
			int OnRun() override
			{
				const int exitCode = m_App.OnRun();
				if (!m_ExitCode)
				{
					m_ExitCode = exitCode;
				}
				return *m_ExitCode;
			}

			// Event handling
			int MainLoop() override
			{
				return m_App.MainLoop();
			}
			void ExitMainLoop() override
			{
				m_App.ExitMainLoop();
			}
			void OnEventLoopEnter(wxEventLoopBase* loop) override
			{
				if (loop)
				{
					m_App.OnEventLoopEnter(*loop);
				}
			}
			void OnEventLoopExit(wxEventLoopBase* loop) override
			{
				if (loop)
				{
					m_App.OnEventLoopExit(*loop);
				}
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
