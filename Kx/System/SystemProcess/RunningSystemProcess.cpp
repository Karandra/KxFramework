#include "stdafx.h"
#include "RunningSystemProcess.h"
#include "Kx/System/NativeAPI.h"
#include "Kx/System/ErrorCodeValue.h"
#include "Kx/System/Private/System.h"
#include "Kx/FileSystem/NativeFileSystem.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include <wx/private/pipestream.h>
#include <wx/private/streamtempinput.h>

#include <PsAPI.h>
#include <WInternl.h>
#include <TlHelp32.h>
#include "Kx/System/UndefWindows.h"

namespace
{
	// Get environment variables of another process
	// http://stackoverflow.com/questions/38297878/get-startupinfo-for-given-process
	// https://msdn.microsoft.com/en-us/library/bb432286(v=vs.85).aspx

	bool SafeTerminateProcess(HANDLE processHandle, uint32_t exitCode) noexcept
	{
		using namespace KxFramework;

		bool isSuccess = false;
		DWORD errorCode = 0;

		HANDLE processHandleDuplicate = nullptr;
		HANDLE remoteThreadHandle = nullptr;
		Utility::CallAtScopeExit atExit = [&]()
		{
			if (processHandleDuplicate)
			{
				::CloseHandle(processHandleDuplicate);
			}
			if (remoteThreadHandle)
			{
				::CloseHandle(remoteThreadHandle);
			}

			if (!isSuccess)
			{
				::SetLastError(errorCode);
			}
		};

		::DuplicateHandle(::GetCurrentProcess(),
						  processHandle,
						  ::GetCurrentProcess(),
						  &processHandleDuplicate,
						  PROCESS_ALL_ACCESS,
						  FALSE,
						  0
		);


		DWORD processExitCode = 0;
		if (::GetExitCodeProcess(processHandleDuplicate ? processHandleDuplicate : processHandle, &processExitCode) && (processExitCode == STILL_ACTIVE))
		{
			DWORD threadID = 0;
			remoteThreadHandle = ::CreateRemoteThread(processHandleDuplicate ? processHandleDuplicate : processHandle,
													  nullptr,
													  0,
													  reinterpret_cast<LPTHREAD_START_ROUTINE>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "ExitProcess")),
													  reinterpret_cast<void*>(static_cast<size_t>(exitCode)),
													  0,
													  &threadID);

			if (!remoteThreadHandle)
			{
				errorCode = ::GetLastError();
			}
		}
		else
		{
			errorCode = ERROR_PROCESS_ABORTED;
		}
		
		if (remoteThreadHandle)
		{
			// Must wait process to terminate to guarantee that it has exited
			::WaitForSingleObject(processHandleDuplicate ? processHandleDuplicate : processHandle, INFINITE);
			isSuccess = true;
		}
		return isSuccess;
	}
}

namespace KxFramework
{
	RunningSystemProcess RunningSystemProcess::GetCurrentProcess()
	{
		RunningSystemProcess process;
		process.m_Handle = ::GetCurrentProcess();
		return process;
	}

	void RunningSystemProcess::Open(uint32_t pid, SystemProcessAccess access)
	{
		m_Handle = ::OpenProcess(System::Private::MapSystemProcessAccess(access), FALSE, pid);
	}
	void RunningSystemProcess::Close()
	{
		if (m_Handle)
		{
			::CloseHandle(m_Handle);
		}
		m_Handle = nullptr;
	}

	bool RunningSystemProcess::IsCurrent() const
	{
		return GetID() == ::GetCurrentProcessId();
	}
	bool RunningSystemProcess::Is64Bit() const
	{
		if (NativeAPI::Kernel32::IsWow64Process && !IsNull())
		{
			BOOL value = FALSE;
			if (NativeAPI::Kernel32::IsWow64Process(m_Handle, &value))
			{
				return !value;
			}
		}
		return false;
	}
	uint32_t RunningSystemProcess::GetID() const
	{
		return ::GetProcessId(m_Handle);
	}
	
	SystemProcessPriority RunningSystemProcess::GetPriority() const
	{
		return System::Private::MapSystemProcessPriority(::GetPriorityClass(m_Handle));
	}
	bool RunningSystemProcess::SetPriority(SystemProcessPriority value)
	{
		if (auto priority = System::Private::MapSystemProcessPriority(value))
		{
			return ::SetPriorityClass(m_Handle, *priority);
		}
		return false;
	}

	bool RunningSystemProcess::IsRunning() const
	{
		DWORD exitCode = 0;
		return ::GetExitCodeProcess(m_Handle, &exitCode) && exitCode == STILL_ACTIVE;
	}
	std::optional<uint32_t> RunningSystemProcess::GetExitCode() const
	{
		DWORD exitCode = STILL_ACTIVE;
		if (::GetExitCodeProcess(m_Handle, &exitCode) && exitCode != STILL_ACTIVE)
		{
			return exitCode;
		}
		return {};
	}
	bool RunningSystemProcess::Terminate(uint32_t exitCode, bool force)
	{
		return force ? ::TerminateProcess(m_Handle, exitCode) : SafeTerminateProcess(m_Handle, exitCode);
	}

	bool RunningSystemProcess::SuspendProcess()
	{
		if (NativeAPI::NtDLL::NtSuspendProcess)
		{
			return NativeAPI::NtDLL::NtSuspendProcess(m_Handle) != 0;
		}
		return false;
	}
	bool RunningSystemProcess::ResumeProcess()
	{
		if (NativeAPI::NtDLL::NtResumeProcess)
		{
			return NativeAPI::NtDLL::NtResumeProcess(m_Handle) != 0;
		}
		return false;
	}

	FSPath RunningSystemProcess::GetExecutablePath() const
	{
		DWORD length = 0;
		wchar_t buffer[INT16_MAX] = {};
		if (::QueryFullProcessImageNameW(m_Handle, 0, buffer, &length))
		{
			return buffer;
		}
		return {};
	}
	FSPath RunningSystemProcess::GetWorkingDirectory() const
	{
		// https://stackoverflow.com/questions/14018280/how-to-get-a-process-working-dir-on-windows
		if (IsCurrent())
		{
			return NativeFileSystem::Get().GetWorkingDirectory();
		}
		return {};
	}
	String RunningSystemProcess::GetExecutableParameters() const
	{
		String commandLine = GetCommandLine();
		if (!commandLine.IsEmpty())
		{
			String parameters;

			FSPath path = GetExecutablePath();
			if (commandLine.StartsWith(path.GetFullPath(), &parameters, StringOpFlag::IgnoreCase))
			{
				return parameters;
			}
		}
		return {};
	}
	String RunningSystemProcess::GetCommandLine() const
	{
		// http://stackoverflow.com/questions/18358150/ntqueryinformationprocess-keep-to-fail

		// PROCESS_QUERY_INFORMATION|PROCESS_VM_READ

		if (NativeAPI::NtDLL::NtQueryInformationProcess)
		{
			PROCESS_BASIC_INFORMATION processInformation = {};
			ULONG processInformationSize = 0;
			if (NativeAPI::NtDLL::NtQueryInformationProcess(m_Handle, PROCESSINFOCLASS::ProcessBasicInformation, &processInformation, sizeof(processInformation), &processInformationSize) >= 0)
			{
				// Read PEB memory block
				SIZE_T read = 0;
				PEB peb = {};
				if (::ReadProcessMemory(m_Handle, processInformation.PebBaseAddress, &peb, sizeof(peb), &read))
				{
					// Get size of command line string
					RTL_USER_PROCESS_PARAMETERS processParameters = {};
					if (::ReadProcessMemory(m_Handle, peb.ProcessParameters, &processParameters, sizeof(processParameters), &read))
					{
						if (processParameters.CommandLine.Length != 0)
						{
							String result;
							const size_t resultLength = (processParameters.CommandLine.Length + 1) / sizeof(wchar_t);
							if (::ReadProcessMemory(m_Handle, processParameters.CommandLine.Buffer, wxStringBuffer(result, resultLength), processParameters.CommandLine.Length, &read))
							{
								return result;
							}
						}
					}
				}
			}
		}
		return {};
	}

	size_t RunningSystemProcess::EnumThreads(std::function<bool(uint32_t)> func) const
	{
		if (IsNull())
		{
			return 0;
		}

		HANDLE snapshotHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (snapshotHandle && snapshotHandle != INVALID_HANDLE_VALUE)
		{
			Utility::CallAtScopeExit atExit = [&]()
			{
				::CloseHandle(snapshotHandle);
			};

			THREADENTRY32 entry = {};
			entry.dwSize = sizeof(entry);
			if (::Thread32First(snapshotHandle, &entry))
			{
				size_t count = 0;
				uint32_t currentPID = GetID();
				bool invokeCallback = true;

				do
				{
					if (entry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(entry.th32OwnerProcessID) && entry.th32OwnerProcessID == currentPID)
					{
						if (invokeCallback)
						{
							count++;
							if (!std::invoke(func, entry.th32ThreadID))
							{
								invokeCallback = false;
							}
						}
					}
					entry.dwSize = sizeof(entry);
				}
				while (::Thread32Next(snapshotHandle, &entry));
				return count;
			}
		}
		return 0;
	}
	size_t RunningSystemProcess::EnumWindows(std::function<bool(void*)> func) const
	{
		std::tuple<decltype(func)&, size_t, uint32_t> callContext = {func, 0, GetID()};
		::EnumWindows([](HWND windowHandle, LPARAM lParam) -> BOOL
		{
			auto& context = *reinterpret_cast<decltype(callContext)*>(lParam);

			DWORD pid = 0;
			::GetWindowThreadProcessId(windowHandle, &pid);

			if (pid != 0 && pid == std::get<2>(context))
			{
				++std::get<1>(context);
				return std::invoke(std::get<0>(context), windowHandle);
			}
			return TRUE;
		}, reinterpret_cast<LPARAM>(&callContext));

		return std::get<1>(callContext);
	}
	uint32_t RunningSystemProcess::GetMainThread() const
	{
		uint32_t result = 0;
		std::optional<FILETIME> earliestTime;

		EnumThreads([&](uint32_t threadID)
		{
			if (HANDLE threadHandle = ::OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, threadID))
			{
				Utility::CallAtScopeExit atExit = [&]()
				{
					::CloseHandle(threadHandle);
				};

				FILETIME creation = {};
				FILETIME exit = {};
				FILETIME kernel = {};
				FILETIME user = {};
				if (::GetThreadTimes(threadHandle, &creation, &exit, &kernel, &user))
				{
					if (!earliestTime)
					{
						earliestTime = creation;
						result = threadID;
					}
					else if (::CompareFileTime(&creation, &*earliestTime) == -1)
					{
						earliestTime = creation;
						result = threadID;
					}
				}
			}
			return true;
		});
		return result;
	}
}
