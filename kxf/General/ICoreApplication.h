#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/RTTI/QueryInterface.h"
class wxApp;
class wxEventLoopBase;
class wxCmdLineParser;

namespace kxf
{
	class KX_API ICoreApplication: public RTTI::Interface<ICoreApplication>
	{
		KxDeclareIID(ICoreApplication, {0x2db9e5b5, 0x29cb, 0x4e8a, {0xb4, 0x59, 0x16, 0xee, 0xb, 0xad, 0x92, 0xdf}});

		public:
			static ICoreApplication* GetInstance() noexcept;
			static void SetInstance(ICoreApplication* instance) noexcept;

		public:
			virtual ~ICoreApplication() = default;

		public:
			virtual void Exit(int exitCode) = 0;
			virtual std::optional<int> GetExitCode() const = 0;
			virtual wxApp& GetWxImpl() = 0;

			// Application information
			virtual String GetName() const = 0;
			virtual void SetName(const String& name) = 0;

			virtual String GetDisplayName() const = 0;
			virtual void SetDisplayName(const String& name) = 0;

			virtual String GetVendorName() const = 0;
			virtual void SetVendorName(const String& name) = 0;

			virtual String GetVendorDisplayName() const = 0;
			virtual void SetVendorDisplayName(const String& name) = 0;

			virtual String GetClassName() const = 0;
			virtual void SetClassName(const String& name) = 0;

			virtual Version GetVersion() const = 0;
			virtual void SetVersion(const Version& version) = 0;

			// Callbacks for application-wide events
			virtual bool OnCreate()
			{
				return true;
			}
			virtual void OnDestroy()
			{
			}

			virtual bool OnInit() = 0;
			virtual void OnExit() = 0;
			virtual int OnRun() = 0;

			// Event handling
			virtual int MainLoop() = 0;
			virtual void ExitMainLoop() = 0;
			virtual int FilterEvent(wxEvent& event) = 0;
			virtual wxEventLoopBase* GetMainLoop() const = 0;
			virtual void HandleEvent(wxEvtHandler& handler, wxEventFunction func, wxEvent& event) const = 0;
			virtual void OnEventLoopEnter(wxEventLoopBase& loop) = 0;
			virtual void OnEventLoopExit(wxEventLoopBase& loop) = 0;
			virtual wxEvtHandler& GetEvtHandler() = 0;

			// Pending events
			virtual void ProcessPendingEvents() = 0;
			virtual void DiscardPendingEvents() = 0;
			virtual bool Yield() = 0;

			virtual void SuspendPendingEventsProcessing() = 0;
			virtual void ResumePendingEventsProcessing() = 0;

			virtual bool IsScheduledForDestruction(const wxObject& object) const = 0;
			virtual void ScheduleForDestruction(wxObject& object) = 0;

			// Exceptions support
			virtual bool OnMainLoopException() = 0;
			virtual void OnUnhandledException() = 0;
			virtual void OnFatalException() = 0;
			virtual void OnAssertFailure(String file, int line, String function, String condition, String message) = 0;

			virtual bool StoreCurrentException() = 0;
			virtual void RethrowStoredException() = 0;

			// Command line
			virtual size_t EnumCommandLineArgs(std::function<bool(String)> func) = 0;
			virtual void OnCommandLineInit(wxCmdLineParser& parser) = 0;
			virtual bool OnCommandLineParsed(wxCmdLineParser& parser) = 0;
			virtual bool OnCommandLineError(wxCmdLineParser& parser) = 0;
			virtual bool OnCommandLineHelp(wxCmdLineParser& parser) = 0;
	};
}
