/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxProcessThread.h"
#include "KxFramework/KxProcess.h"
#include "Kx/General/StringFormater.h"
#include "Kx/FileSystem/NativeFileSystem.h"
#include <wx/private/pipestream.h>
#include <wx/private/streamtempinput.h>

bool KxProcessThread::PrepareRedirection(KxProcessPipe& pipeIn, KxProcessPipe& pipeOut, KxProcessPipe& pipeError, STARTUPINFOW& startupInfo)
{
	if (pipeIn.Create() && pipeOut.Create() && pipeError.Create())
	{
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;
		startupInfo.hStdInput = pipeIn[KxProcessPipe::Read];
		startupInfo.hStdOutput = pipeOut[KxProcessPipe::Write];
		startupInfo.hStdError = pipeError[KxProcessPipe::Write];

		// We must set the handles to those sides of std* pipes that we won't
		// in the child to be non-inheritable. We must do this before launching
		// the child process as otherwise these handles will be inherited by
		// the child which will never close them and so the pipe will not
		// return ERROR_BROKEN_PIPE if the parent or child exits unexpectedly
		// causing the remaining process to potentially become deadlocked in
		// ReadFile() or WriteFile().
		::SetHandleInformation(pipeIn[KxProcessPipe::Write], HANDLE_FLAG_INHERIT, 0);
		::SetHandleInformation(pipeOut[KxProcessPipe::Read], HANDLE_FLAG_INHERIT, 0);
		::SetHandleInformation(pipeError[KxProcessPipe::Read], HANDLE_FLAG_INHERIT, 0);

		return true;
	}
	return false;
}
bool KxProcessThread::EndRedirection(KxProcessPipe& pipeIn, KxProcessPipe& pipeOut, KxProcessPipe& pipeError, wxStreamTempInputBuffer& outBuffer, wxStreamTempInputBuffer& errorBuffer)
{
	if (m_IsProcessCreated)
	{
		if (IsProcessAlive())
		{
			// We can now initialize the wxStreams
			m_EventHandler->RIO_CloseStreams();
			m_EventHandler->m_RIO_StreamOut = new wxPipeInputStream(pipeOut.Detach(KxProcessPipe::Read));
			m_EventHandler->m_RIO_StreamError = new wxPipeInputStream(pipeError.Detach(KxProcessPipe::Read));
			m_EventHandler->m_RIO_StreamIn = new wxPipeOutputStream(pipeIn.Detach(KxProcessPipe::Write));

			// The input buffer outBuffer is connected to stdout, this is why it is called outBuffer and not tInBuffer
			outBuffer.Init(m_EventHandler->m_RIO_StreamOut);
			errorBuffer.Init(m_EventHandler->m_RIO_StreamError);

			return true;
		}
	}
	else
	{
		// Close the other handles too
		::CloseHandle(pipeIn.Detach(KxProcessPipe::Write));
		::CloseHandle(pipeOut.Detach(KxProcessPipe::Read));
		::CloseHandle(pipeError.Detach(KxProcessPipe::Read));
	}
	return false;
}
void* KxProcessThread::ConvertEnvTable(const KxProcessEnvMap& envTable, wxMemoryBuffer& buffer)
{
	if (!envTable.empty())
	{
		for (const auto& element: envTable)
		{
			// name=value\0
			buffer.AppendData(element.first.wc_str(), element.first.Length() * sizeof(WCHAR));
			buffer.AppendData(L"=", sizeof(WCHAR));
			buffer.AppendData(element.second.wc_str(), element.second.Length() * sizeof(WCHAR));
			buffer.AppendData(L"\0", sizeof(WCHAR));
		}
		buffer.AppendData(L"\0", sizeof(WCHAR));
		return buffer.GetData();
	}
	return nullptr;
}
void KxProcessThread::SendEventSyncAsync(wxEvent* event)
{
	if (m_RunMode == KxPROCESS_RUN_SYNC)
	{
		m_EventHandler->ProcessEvent(*event);
		delete event;
	}
	else
	{
		m_EventHandler->QueueEvent(event);
	}
}

bool KxProcessThread::CreateProcess(PROCESS_INFORMATION& processInfo)
{
	using namespace KxFramework;

	// Configure startup
	m_IsProcessCreated = false;
	STARTUPINFOW startupInfo = {0};
	startupInfo.cb = sizeof(startupInfo);

	startupInfo.dwFlags = STARTF_USESHOWWINDOW|STARTF_FORCEOFFFEEDBACK;
	startupInfo.wShowWindow = m_HideUI ? SW_HIDE : SW_SHOWNORMAL;

	// Configure I/O redirection
	KxProcessPipe pipeIn, pipeOut, pipeError;
	if (m_IsRedirectedIO && !PrepareRedirection(pipeIn, pipeOut, pipeError, startupInfo))
	{
		return false;
	}
	else
	{
		// Process creation flags
		const DWORD priorityMask = REALTIME_PRIORITY_CLASS|NORMAL_PRIORITY_CLASS|IDLE_PRIORITY_CLASS|HIGH_PRIORITY_CLASS|BELOW_NORMAL_PRIORITY_CLASS|ABOVE_NORMAL_PRIORITY_CLASS;
		DWORD processFlags = CREATE_SUSPENDED|CREATE_UNICODE_ENVIRONMENT|(m_EventHandler->m_Priority & priorityMask);
		if (m_IsRedirectedIO)
		{
			processFlags |= CREATE_NO_WINDOW;
		}

		// Add user flags
		processFlags |= m_EventHandler->m_ProcessCreationFlags;

		// Process environment
		wxMemoryBuffer processEnvBuffer;
		void* processEnv = ConvertEnvTable(m_EventHandler->m_EnvironmentTable, processEnvBuffer);

		// Process command line
		wxString commandLine = m_EventHandler->m_Arguments;
		if (commandLine.IsEmpty())
		{
			commandLine = String::Format(wxS("\"%1\""), m_EventHandler->m_ExecutablePath);
		}
		else
		{
			commandLine = String::Format(wxS("\"%1\" %2"), m_EventHandler->m_ExecutablePath, commandLine);
		}

		const size_t commandLineBufferSize = commandLine.length() * sizeof(wchar_t) + sizeof(wchar_t);
		wxMemoryBuffer commandLineBuffer(commandLineBufferSize);
		commandLineBuffer.AppendData(commandLine.wc_str(), commandLineBufferSize);

		// Working folder
		LPCWSTR workingFolderPtr = nullptr;
		wxString workingFolder;
		if (m_EventHandler->m_IsWorkingFolderInExecutablePath)
		{
			workingFolder = m_EventHandler->m_ExecutablePath.BeforeLast('\\');
			if (workingFolder.IsEmpty())
			{
				workingFolder = KxFramework::NativeFileSystem::Get().GetWorkingDirectory().GetFullPath();
			}
			workingFolderPtr = workingFolder.wc_str();
		}
		else if (m_EventHandler->m_WorkingFolder.IsEmpty())
		{
			workingFolderPtr = nullptr;
		}
		else
		{
			workingFolder = m_EventHandler->m_WorkingFolder;
			workingFolderPtr = workingFolder.wc_str();
		}

		m_EventHandler->m_ProcessCreationLastError = 0;
		m_IsProcessCreated = ::CreateProcessW(m_EventHandler->m_ExecutablePath.wc_str(),
											  static_cast<wchar_t*>(commandLineBuffer.GetData()),
											  nullptr,
											  nullptr,
											  m_EventHandler->IsRedirected(),
											  processFlags,
											  processEnv,
											  workingFolderPtr,
											  &startupInfo,
											  &processInfo
		);
		m_EventHandler->m_ProcessCreationLastError = ::GetLastError();
		m_EventHandler->m_ProcessCreationStatus = m_IsProcessCreated;

		if (m_IsProcessCreated)
		{
			m_EventHandler->m_PID = processInfo.dwProcessId;
			m_EventHandler->m_ExitCode = STILL_ACTIVE;
			::ResumeThread(processInfo.hThread);

			WaitProcessIdle(processInfo);
			WaitProcessEnd(processInfo);

			if (m_IsRedirectedIO)
			{
				wxStreamTempInputBuffer outBuffer, errorBuffer;
				EndRedirection(pipeIn, pipeOut, pipeError, outBuffer, errorBuffer);
			}
			CloseThreadAndProcess(processInfo);
		}
		return m_IsProcessCreated;
	}
}
void KxProcessThread::CloseThreadAndProcess(PROCESS_INFORMATION& processInfo)
{
	if (processInfo.hThread != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(processInfo.hThread);
		processInfo.hThread = INVALID_HANDLE_VALUE;
	}
	if (processInfo.hProcess != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(processInfo.hProcess);
		processInfo.hProcess = INVALID_HANDLE_VALUE;
	}
}
bool KxProcessThread::WaitProcessEnd(PROCESS_INFORMATION& processInfo)
{
	if (IsProcessAlive() && m_EventHandler->IsOptionEnabled(KxPROCESS_WAIT_END))
	{
		if (::WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_OBJECT_0)
		{
			DWORD exitCode = (DWORD)-1;
			if (::GetExitCodeProcess(processInfo.hProcess, &exitCode))
			{
				m_EventHandler->m_ExitCode = exitCode;
			}

			SendProcessEndEvent();
			return true;
		}
	}
	return false;
}
bool KxProcessThread::WaitProcessIdle(PROCESS_INFORMATION& processInfo)
{
	if (IsProcessAlive() && m_EventHandler->IsOptionEnabled(KxPROCESS_WAIT_INPUT_IDLE))
	{
		DWORD status = ::WaitForInputIdle(processInfo.hProcess, INFINITE);
		if (status == 0)
		{
			SendProcessIdleEvent();
			return true;
		}
	}
	return false;
}
void KxProcessThread::SendProcessEndEvent()
{
	wxCriticalSectionLocker lock(m_EndCS);
	if (IsProcessAlive())
	{
		m_EndSignaled = true;

		wxProcessEvent* event = new wxProcessEvent(wxID_NONE, m_EventHandler->m_PID, m_EventHandler->m_ExitCode);
		event->SetEventType(KxEVT_PROCESS_END);
		event->SetEventObject(m_EventHandler);
		SendEventSyncAsync(event);

		if (m_EventHandler->IsOptionEnabled(KxPROCESS_DETACHED))
		{
			delete m_EventHandler;
			m_EventHandler = nullptr;
			return;
		}
	}
}
void KxProcessThread::SendProcessIdleEvent()
{
	wxProcessEvent* event = new wxProcessEvent(wxID_NONE, m_EventHandler->m_PID, m_EventHandler->m_ExitCode);
	event->SetEventType(KxEVT_PROCESS_IDLE);
	event->SetEventObject(m_EventHandler);
	SendEventSyncAsync(event);
}

KxProcessThread::ExitCode KxProcessThread::Entry()
{
	PROCESS_INFORMATION processInfo = {0};
	if (m_Handle != INVALID_HANDLE_VALUE)
	{
		processInfo.hProcess = m_Handle;

		WaitProcessIdle(processInfo);
		WaitProcessEnd(processInfo);
		CloseThreadAndProcess(processInfo);
	}
	else
	{
		CreateProcess(processInfo);
	}

	// This call will send end process event only if the event wasn't already sent.
	SendProcessEndEvent();
	return 0;
}

KxProcessThread::KxProcessThread(KxProcess* eventHandler, KxProcessWaitMode runMode, bool hideUI, HANDLE processHandle)
	:m_EventHandler(eventHandler),
	m_RunMode(runMode),
	m_HideUI(hideUI),
	m_Handle(processHandle),
	m_IsRedirectedIO(eventHandler->IsRedirected())
{
}
KxProcessThread::~KxProcessThread()
{
}
