/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/General/WithOptions.h"
#include "Kx/System/UndefWindows.h"
#include "KxEvent.h"
#include <wx/process.h>
#include <wx/stream.h>
class KxProcessThread;
class KxProcessPipe;
class wxPipeOutputStream;
class wxPipeInputStream;

KxEVENT_DECLARE_GLOBAL(wxProcessEvent, PROCESS_END);
KxEVENT_DECLARE_GLOBAL(wxProcessEvent, PROCESS_IDLE);

enum KxProcessOptions
{
	KxPROCESS_NONE = 0,
	KxPROCESS_DETACHED = 1 << 0,
	KxPROCESS_WAIT_END = 1 << 1,
	KxPROCESS_WAIT_INPUT_IDLE = 1 << 2,

	KxPROCESS_DEFAULT_OPTIONS = KxPROCESS_NONE,
};

enum KxProcessWaitMode
{
	KxPROCESS_RUN_SYNC,
	KxPROCESS_RUN_ASYNC,
};

using KxProcessEnvMap = std::unordered_map<wxString, wxString>;
class KX_API KxProcess: public wxEvtHandler, public KxFramework::WithOptions<KxProcessOptions, KxPROCESS_DEFAULT_OPTIONS>
{
	friend class KxProcessThread;

	public:
		using HWNDVector = std::vector<HWND>;

		static const DWORD DefaultExitCode = (DWORD)-1;
		static const DWORD DefaultPID = 0;

	private:
		wxString m_ExecutablePath;
		wxString m_Arguments;
		wxString m_WorkingFolder;
		bool m_IsWorkingFolderInExecutablePath = false;
		KxProcessEnvMap m_EnvironmentTable;
		DWORD m_PID = DefaultPID;
		DWORD m_ExitCode = DefaultExitCode;
		DWORD m_Priority = NORMAL_PRIORITY_CLASS;

		bool m_ProcessCreationStatus = false;
		DWORD m_ProcessCreationLastError = 0;
		DWORD m_ProcessCreationFlags = 0;
		bool m_ProcessCreationFlagsOverride = false;

		// I/O redirection part
		bool m_RIO_Enabled = false;
		wxPipeOutputStream* m_RIO_StreamIn = nullptr;
		wxPipeInputStream* m_RIO_StreamOut = nullptr;
		wxPipeInputStream* m_RIO_StreamError = nullptr;

		// Windows hooks
		#if defined RtCFunction
		RtCFunction* WH_Callback = nullptr;
		HHOOK WH_CreateWindowHookHandle = nullptr;
		#endif

	private:
		#if defined RtCFunction
		virtual bool OnDynamicBind(wxDynamicEventTableEntry& entry) override;
		virtual void OnDynamicUnbind(wxDynamicEventTableEntry& entry) override;
		#endif

		bool IsDetached() const
		{
			return IsOptionEnabled(KxPROCESS_DETACHED);
		}
		
		// IO Redirection functions
		static void RIO_ReadStream(wxInputStream* stream, wxMemoryBuffer& buffer);
		static wxString RIO_StreamDataToString(const wxMemoryBuffer& buffer);
		void RIO_CloseStreams();

		// Window hooks
		#if defined RtCFunction
		bool WH_RegisterForCreateWindow(DWORD nThreadID);
		void WH_UnRegisterForCreateWindow();
		#endif

	public:
		static BOOL SafeTerminateProcess(HANDLE processHandle, UINT exitCode);
		static KxUInt32Vector EnumProcesses(bool enumX64 = false);
		static DWORD GetMainThread(const KxUInt32Vector& threadIDsList);
		static DWORD GetCurrentThreadID()
		{
			return ::GetCurrentThreadId();
		}

	public:
		KxProcess();
		KxProcess(DWORD pid);
		KxProcess(const wxString& executablePath, const wxString& arguments = {}, const wxString& workingFolder = {});
		virtual ~KxProcess();

		int Run(KxProcessWaitMode waitMode = KxPROCESS_RUN_ASYNC, bool hideUI = false);
		void SetProcessCreationFlags(DWORD flags, bool overrideDefaults = false)
		{
			m_ProcessCreationFlags = flags;
			m_ProcessCreationFlagsOverride = overrideDefaults;
		}
		bool Find();
		DWORD GetPID() const;
		DWORD GetExitCode() const;
		bool GetRunStatus() const
		{
			return m_ProcessCreationStatus;
		}
		DWORD GetRunLastErrorCode() const
		{
			return m_ProcessCreationLastError;
		}
		DWORD GetPriority() const;
		bool SetPriority(DWORD priority = NORMAL_PRIORITY_CLASS);
		bool IsExist() const;
		bool Is64Bit() const;
		bool Terminate(DWORD code = DefaultExitCode, bool force = false);
		
		wxString GetImageName() const;
		wxString GetCommandLine() const;
		
		wxString GetExecutablePath() const
		{
			return m_ExecutablePath;
		}
		void SetExecutablePath(const wxString& value)
		{
			m_ExecutablePath = value;
		}
		
		wxString GetArguments() const
		{
			return m_Arguments;
		}
		void SetArguments(const wxString& value)
		{
			m_Arguments = value;
		}
		
		wxString GetWorkingFolder() const
		{
			return m_WorkingFolder;
		}
		void SetWorkingFolder(const wxString& value)
		{
			m_WorkingFolder = value;
			m_IsWorkingFolderInExecutablePath = false;
		}
		void SetWorkingFolder()
		{
			m_IsWorkingFolderInExecutablePath = true;
		}
		
		const KxProcessEnvMap& GetEnvironment() const;
		void SetEnvironment(const wxString& name, const wxString& value)
		{
			m_EnvironmentTable[name] = value;
		}
		void ClearEnvironment()
		{
			m_EnvironmentTable.clear();
		}
		
		bool Attach(KxProcessWaitMode waitMode = KxPROCESS_RUN_ASYNC); // There can't be KxPROCESS_WAIT_NO
		KxUInt32Vector EnumThreads() const;
		HWNDVector EnumWindows() const;

		bool IsRedirected() const
		{
			return m_RIO_Enabled;
		}
		void Redirect()
		{
			m_RIO_Enabled = true;
		}
		void SetRedirectionEnabled(bool value)
		{
			m_RIO_Enabled = value;
		}
		wxString GetStdOut() const;
		wxString GetStdErr() const;
};
