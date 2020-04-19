/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "stdafx.h"
#include "KxFramework/KxProcess.h"
#include "KxFramework/KxProcessThread.h"
#include <PsAPI.h>
#include <WInternl.h>
#include <TlHelp32.h>
#include <wx/private/pipestream.h>
#include <wx/private/streamtempinput.h>
#include "Kx/FileSystem/FileOperations.h"
#include "Kx/System/NativeAPI.h"
#include "Kx/System/UndefWindows.h"
#pragma warning (disable: 4312)

using namespace KxFramework;

KxEVENT_DEFINE_GLOBAL_AS(wxProcessEvent, PROCESS_END, wxEVT_END_PROCESS);
KxEVENT_DEFINE_GLOBAL(wxProcessEvent, PROCESS_IDLE);

void KxProcess::RIO_ReadStream(wxInputStream* stream, wxMemoryBuffer& buffer)
{
	if (stream != nullptr)
	{
		while (stream->CanRead())
		{
			buffer.AppendByte(stream->GetC());
		}
	}
}
wxString KxProcess::RIO_StreamDataToString(const wxMemoryBuffer& buffer)
{
	return wxString((const char*)buffer.GetData(), wxConvUTF8, buffer.GetDataLen());
}
void KxProcess::RIO_CloseStreams()
{
	delete m_RIO_StreamIn;
	delete m_RIO_StreamOut;
	delete m_RIO_StreamError;

	m_RIO_StreamIn = nullptr;
	m_RIO_StreamOut = nullptr;
	m_RIO_StreamError = nullptr;
}

BOOL KxProcess::SafeTerminateProcess(HANDLE processHandle, UINT exitCode)
{
	HANDLE processHandleDuplicate = INVALID_HANDLE_VALUE;
	bool wasDuplicated = ::DuplicateHandle(::GetCurrentProcess(),
										   processHandle,
										   ::GetCurrentProcess(),
										   &processHandleDuplicate,
										   PROCESS_ALL_ACCESS,
										   FALSE,
										   0
	);

	
	DWORD processExitCode = 0;
	DWORD errorCode = 0;
	HANDLE remoteThreadHandle = nullptr;
	if (::GetExitCodeProcess(wasDuplicated ? processHandleDuplicate : processHandle, &processExitCode) && (processExitCode == STILL_ACTIVE))
	{
		DWORD threadID = 0;
		remoteThreadHandle = ::CreateRemoteThread((wasDuplicated) ? processHandleDuplicate : processHandle,
												  nullptr,
												  0,
												  reinterpret_cast<LPTHREAD_START_ROUTINE>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "ExitProcess")),
												  reinterpret_cast<void*>(exitCode),
												  0,
												  & threadID);

		if (!remoteThreadHandle)
		{
			errorCode = ::GetLastError();
		}
	}
	else
	{
		errorCode = ERROR_PROCESS_ABORTED;
	}

	bool isSuccess = false;
	if (remoteThreadHandle)
	{
		// Must wait process to terminate to guarantee that it has exited...
		::WaitForSingleObject(wasDuplicated ? processHandleDuplicate : processHandle, INFINITE);
		::CloseHandle(remoteThreadHandle);
		isSuccess = true;
	}

	if (wasDuplicated)
	{
		CloseHandle(processHandleDuplicate);
	}

	if (!isSuccess)
	{
		SetLastError(errorCode);
	}
	return isSuccess;
}
DWORD KxProcess::GetMainThread(const std::vector<uint32_t>& threadIDsList)
{
	DWORD mainThreadID = 0;
	for (size_t i = 0; i < threadIDsList.size(); i++)
	{
		DWORD id = threadIDsList[i];

		FILETIME earliestTime = {0};
		HANDLE threadHandle = ::OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, id);
		if (threadHandle)
		{
			FILETIME tCreation, t1, t2, t3;
			if (::GetThreadTimes(threadHandle, &tCreation, &t1, &t2, &t3))
			{
				if (i == 0)
				{
					earliestTime = tCreation;
					mainThreadID = id;
				}
				else if (::CompareFileTime(&tCreation, &earliestTime) == -1)
				{
					earliestTime = tCreation;
					mainThreadID = id;
				}
			}
			::CloseHandle(threadHandle);
		}
	}
	return mainThreadID;
}

KxProcess::KxProcess()
{
}
KxProcess::KxProcess(DWORD pid)
	:m_PID(pid)
{
	if (pid == 0)
	{
		m_PID = GetCurrentProcessId();
	}
	m_ExecutablePath = GetImageName();
}
KxProcess::KxProcess(const wxString& executablePath, const wxString& arguments, const wxString& workingFolder)
	:m_ExecutablePath(executablePath), m_Arguments(arguments), m_WorkingFolder(workingFolder)
{
}
KxProcess::~KxProcess()
{
	RIO_CloseStreams();

	#if defined RtCFunction
	WH_UnRegisterForCreateWindow();
	#endif
}

int KxProcess::Run(KxProcessWaitMode waitMode, bool hideUI)
{
	m_PID = DefaultPID;
	m_ExitCode = DefaultExitCode;
	m_ProcessCreationStatus = false;

	if (waitMode == KxPROCESS_RUN_ASYNC)
	{
		auto thread = new KxProcessThread(this, waitMode, hideUI);
		if (thread->Run() != wxTHREAD_NO_ERROR)
		{
			delete thread;
			if (IsDetached())
			{
				delete this;
			}
		}
	}
	else if (waitMode == KxPROCESS_RUN_SYNC)
	{
		KxProcessThread(this, waitMode, hideUI).RunHere();
	}
	else
	{
		if (IsDetached())
		{
			delete this;
		}
	}
	return m_ExitCode;
}
bool KxProcess::Find()
{
	return false;
}
DWORD KxProcess::GetPID() const
{
	return m_PID;
}
DWORD KxProcess::GetExitCode() const
{
	HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, m_PID);
	if (processHandle)
	{
		DWORD exitCode = 0;
		::GetExitCodeProcess(processHandle, &exitCode);
		::CloseHandle(processHandle);

		return exitCode;
	}
	return m_ExitCode;
}

DWORD KxProcess::GetPriority() const
{
	DWORD ret = m_Priority;
	HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, GetPID());
	if (processHandle)
	{
		ret = ::GetPriorityClass(processHandle);
		::CloseHandle(processHandle);
	}
	return ret;
}
bool KxProcess::SetPriority(DWORD priority)
{
	// Clear priority value
	if (priority & REALTIME_PRIORITY_CLASS)
	{
		m_Priority = REALTIME_PRIORITY_CLASS;
	}
	else if (priority & HIGH_PRIORITY_CLASS)
	{
		m_Priority = HIGH_PRIORITY_CLASS;
	}
	else if (priority & ABOVE_NORMAL_PRIORITY_CLASS)
	{
		m_Priority = ABOVE_NORMAL_PRIORITY_CLASS;
	}
	else if (priority & NORMAL_PRIORITY_CLASS)
	{
		m_Priority = NORMAL_PRIORITY_CLASS;
	}
	else if (priority & BELOW_NORMAL_PRIORITY_CLASS)
	{
		m_Priority = BELOW_NORMAL_PRIORITY_CLASS;
	}
	else if (priority & IDLE_PRIORITY_CLASS)
	{
		m_Priority = IDLE_PRIORITY_CLASS;
	}

	bool ret = true;
	HANDLE processHandle = ::OpenProcess(PROCESS_SET_INFORMATION, false, GetPID());
	if (processHandle)
	{
		ret = ::SetPriorityClass(processHandle, priority);
		::CloseHandle(processHandle);
	}
	return ret;
}

bool KxProcess::IsExist() const
{
	return GetExitCode() == STILL_ACTIVE;
}
bool KxProcess::Is64Bit() const
{
	HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, GetPID());
	if (processHandle)
	{
		BOOL Is64Bit = FALSE;
		if (NativeAPI::Kernel32::IsWow64Process)
		{
			if (NativeAPI::Kernel32::IsWow64Process(processHandle, &Is64Bit))
			{
				Is64Bit = !Is64Bit;
			}
		}
		::CloseHandle(processHandle);
		return Is64Bit;
	}
	return false;
}
bool KxProcess::Terminate(DWORD code, bool force)
{
	bool ret = false;
	HANDLE processHandle = ::OpenProcess(PROCESS_TERMINATE, false, GetPID());
	if (processHandle)
	{
		if (force)
		{
			ret = ::TerminateProcess(processHandle, code);
		}
		else
		{
			ret = SafeTerminateProcess(processHandle, code);
		}
		::CloseHandle(processHandle);
	}
	return ret;
}

wxString KxProcess::GetImageName() const
{
	wxString out = m_ExecutablePath;
	HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, GetPID());
	if (processHandle)
	{
		DWORD length = INT16_MAX;
		::QueryFullProcessImageNameW(processHandle, 0, wxStringBuffer(out, length), &length);
		out.Shrink();
		::CloseHandle(processHandle);
	}
	return out;
}
wxString KxProcess::GetCommandLine() const
{
	// http://stackoverflow.com/questions/18358150/ntqueryinformationprocess-keep-to-fail
	
	wxString result;
	if (NativeAPI::NtDLL::NtQueryInformationProcess)
	{
		HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, false, GetPID());
		if (processHandle)
		{
			PROCESS_BASIC_INFORMATION processInformation = {0};
			ULONG retLength = 0;
			if (NativeAPI::NtDLL::NtQueryInformationProcess(processHandle, PROCESSINFOCLASS::ProcessBasicInformation, &processInformation, sizeof(processInformation), &retLength) >= 0)
			{
				// Read PEB memory block
				SIZE_T read = 0;
				PEB PEB = {0};
				if (::ReadProcessMemory(processHandle, processInformation.PebBaseAddress, &PEB, sizeof(PEB), &read))
				{
					// Get size of command line string
					RTL_USER_PROCESS_PARAMETERS processParameters = {0};
					if (::ReadProcessMemory(processHandle, PEB.ProcessParameters, &processParameters, sizeof(processParameters), &read))
					{
						if (processParameters.CommandLine.Length != 0)
						{
							DWORD needLength = (processParameters.CommandLine.Length + 1) / sizeof(WCHAR);
							::ReadProcessMemory(processHandle, processParameters.CommandLine.Buffer, wxStringBuffer(result, needLength), processParameters.CommandLine.Length, &read);
							result.StartsWith(m_ExecutablePath, &result);
							
							using namespace KxFramework;

							String temp = std::move(result);
							temp.Trim();
							temp.Trim(StringOpFlag::FromEnd);
							result = std::move(temp);
						}
					}
				}
			}
			::CloseHandle(processHandle);
		}
	}
	return result;
}
const KxProcessEnvMap& KxProcess::GetEnvironment() const
{
	// http://stackoverflow.com/questions/38297878/get-startupinfo-for-given-process
	// https://msdn.microsoft.com/en-us/library/bb432286(v=vs.85).aspx
	return m_EnvironmentTable;
}

bool KxProcess::Attach(KxProcessWaitMode waitMode)
{
	return false;
}
std::vector<uint32_t> KxProcess::EnumThreads() const
{
	std::vector<uint32_t> array;
	HANDLE snapshotHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snapshotHandle != INVALID_HANDLE_VALUE)
	{
		THREADENTRY32 entry = {0};
		entry.dwSize = sizeof(entry);
		if (::Thread32First(snapshotHandle, &entry))
		{
			do
			{
				if (entry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(entry.th32OwnerProcessID) && entry.th32OwnerProcessID == GetPID())
				{
					array.push_back(entry.th32ThreadID);
				}
				entry.dwSize = sizeof(entry);
			}
			while (::Thread32Next(snapshotHandle, &entry));
		}
		::CloseHandle(snapshotHandle);
	}
	return array;
}
KxProcess::HWNDVector KxProcess::EnumWindows() const
{
	struct Callback
	{
		const KxProcess* self;
		HWNDVector List;
	};
	Callback info;
	info.self = this;

	::EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL
	{
		Callback* info = (Callback*)lParam;

		DWORD pid = DefaultPID;
		::GetWindowThreadProcessId(hWnd, &pid);
		if (pid != DefaultPID && pid == info->self->GetPID())
		{
			info->List.push_back(hWnd);
		}
		return TRUE;
	}, (LPARAM)&info);

	return info.List;
}

wxString KxProcess::GetStdOut() const
{
	if (IsRedirected() && m_RIO_StreamOut)
	{
		wxMemoryBuffer buffer;
		RIO_ReadStream(m_RIO_StreamOut, buffer);
		return RIO_StreamDataToString(buffer);
	}
	return {};
}
wxString KxProcess::GetStdErr() const
{
	if (IsRedirected() && m_RIO_StreamError)
	{
		wxMemoryBuffer buffer;
		RIO_ReadStream(m_RIO_StreamError, buffer);
		return RIO_StreamDataToString(buffer);
	}
	return {};
}
