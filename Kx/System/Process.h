#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include "Kx/FileSystem/FSPath.h"
#include "Kx/RTTI/QueryInterface.h"
#include <unordered_map>
class wxPipeOutputStream;
class wxPipeInputStream;

namespace KxFramework
{
	enum class SystemProcessPriority
	{
		None = -1,

		Idle,
		BelowNormal,
		Normal,
		AboveNormal,
		High,
		Realtime,
	};
	enum class SystemProcessAccess
	{
		None = 0,

		CreateProcess = 1 << 0,
		CreateThread = 1 << 1,
		QueryInformation = 1 << 2,
		QueryLimitedInformation = 1 << 3,
		SetInformation = 1 << 4,
		SuspendResume = 1 << 5,
		Synchronize = 1 << 6,
		Terminate = 1 << 7,
		VMOperation = 1 << 8,
		VMRead = 1 << 9,
		VMWrite = 1 << 10,

		Everything = CreateProcess|CreateThread|QueryInformation|QueryLimitedInformation|SetInformation|SuspendResume|Terminate|Synchronize|VMOperation|VMRead|VMWrite
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(SystemProcessAccess);
	}
}

namespace KxFramework
{
	class KX_API ISystemProcess: public RTTI::Interface<ISystemProcess>
	{
		KxDecalreIID(ISystemProcess, {0x3f3562cb, 0x5674, 0x4662, {0xa0, 0x45, 0x93, 0x28, 0x24, 0xfc, 0x33, 0x4c}});

		public:
			virtual ~ISystemProcess() = default;

		public:
			virtual bool IsNull() const = 0;
			virtual bool DoesExist() const = 0;
			virtual bool IsCurrent() const = 0;
			virtual bool Is64Bit() const = 0;
			virtual uint32_t GetID() const = 0;

			virtual SystemProcessPriority GetPriority() const = 0;
			virtual bool SetPriority(SystemProcessPriority value) = 0;

			virtual bool IsRunning() const = 0;
			virtual std::optional<uint32_t> GetExitCode() const = 0;
			virtual bool Terminate(uint32_t exitCode, bool force = false) = 0;

			virtual FSPath GetExecutablePath() const = 0;
			virtual FSPath GetWorkingDirectory() const = 0;
			virtual String GetExecutableParameters() const = 0;

			virtual size_t EnumThreads(std::function<bool(uint32_t)> func) const = 0;
			virtual size_t EnumWindows(std::function<bool(void*)> func) const = 0;
			virtual uint32_t GetMainThread() const = 0;

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}
	};
}

namespace KxFramework
{
	class KX_API RunningSystemProcess: public ISystemProcess
	{
		public:
			static RunningSystemProcess GetCurrentProcess();

		private:
			void* m_Handle = nullptr;

		private:
			void Open(uint32_t pid, SystemProcessAccess access);
			void Close();

		public:
			RunningSystemProcess() = default;
			RunningSystemProcess(uint32_t pid, SystemProcessAccess access)
			{
				Open(pid, access);
			}
			RunningSystemProcess(const RunningSystemProcess&) = delete;
			RunningSystemProcess(RunningSystemProcess&& other)
			{
				*this = std::move(other);
			}
			~RunningSystemProcess()
			{
				Close();
			}

		public:
			bool IsNull() const override
			{
				return m_Handle == nullptr;
			}
			bool DoesExist() const override
			{
				return m_Handle != nullptr;
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override;
			uint32_t GetID() const override;

			SystemProcessPriority GetPriority() const override;
			bool SetPriority(SystemProcessPriority value) override;

			void* GetHandle() const
			{
				return m_Handle;
			}
			void AttachHandle(void* handle)
			{
				Close();
				m_Handle = handle;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle;
				m_Handle = nullptr;
				return handle;
			}

			bool IsRunning() const override;
			std::optional<uint32_t> GetExitCode() const override;
			bool Terminate(uint32_t exitCode, bool force = false) override;

			FSPath GetExecutablePath() const override;
			FSPath GetWorkingDirectory() const override;
			String GetExecutableParameters() const override;
			String GetCommandLine() const;

			size_t EnumThreads(std::function<bool(uint32_t)> func) const override;
			size_t EnumWindows(std::function<bool(void*)> func) const override;
			uint32_t GetMainThread() const override;

		public:
			RunningSystemProcess& operator=(const RunningSystemProcess&) = delete;
			RunningSystemProcess& operator=(RunningSystemProcess&& other)
			{
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}
	};
}

namespace KxFramework
{
	class KX_API SystemProcessInfo: public ISystemProcess
	{
		public:
			static SystemProcessInfo GetCurrentProcess();

		private:
			std::unordered_map<String, String> m_Environment;
			FSPath m_ExecutablePath;
			FSPath m_WorkingDirectory;
			String m_Parameters;
			uint32_t m_PID = 0;

		public:
			SystemProcessInfo() = default;
			SystemProcessInfo(uint32_t pid)
				:m_PID(pid)
			{
			}
			SystemProcessInfo(const SystemProcessInfo&) = default;
			SystemProcessInfo(SystemProcessInfo&&) = default;

		public:
			bool IsNull() const override
			{
				return m_PID == 0;
			}
			bool DoesExist() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).DoesExist();
			}
			bool IsCurrent() const override;
			bool Is64Bit() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).Is64Bit();
			}
			uint32_t GetID() const override
			{
				return m_PID;
			}

			SystemProcessPriority GetPriority() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).GetPriority();
			}
			bool SetPriority(SystemProcessPriority value) override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::SetInformation).SetPriority(value);
			}

			bool IsRunning() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::QueryLimitedInformation).IsRunning();
			}
			std::optional<uint32_t> GetExitCode() const override
			{
				RunningSystemProcess process(m_PID, SystemProcessAccess::QueryLimitedInformation);
				if (process)
				{
					return process.GetExitCode();
				}
				return {};
			}
			bool Terminate(uint32_t exitCode, bool force = false) override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::Terminate).Terminate(exitCode, force);
			}

			FSPath GetExecutablePath() const override
			{
				return m_ExecutablePath;
			}
			void SetExecutablePath(FSPath path)
			{
				m_ExecutablePath = std::move(path);
			}

			FSPath GetWorkingDirectory() const override
			{
				return m_WorkingDirectory;
			}
			void SetWorkingDirectory(FSPath directory)
			{
				m_WorkingDirectory = std::move(directory);
			}

			String GetExecutableParameters() const override
			{
				return m_Parameters;
			}
			void SetExecutableParameters(String parameters)
			{
				m_Parameters = std::move(parameters);
			}

			size_t EnumEnvironemntVariables(std::function<bool(const String&, const String&)> func) const
			{
				size_t count = 0;
				for (const auto&[name, value]: m_Environment)
				{
					count++;
					if (!std::invoke(func, name, value))
					{
						break;
					}
				}
				return count;
			}
			void SetEnvironmentVariable(const String& name, const String& value)
			{
				m_Environment.insert_or_assign(name, value);
			}
			void ClearEnvironmentVariables()
			{
				m_Environment.clear();
			}

			size_t EnumThreads(std::function<bool(uint32_t)> func) const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::None).EnumThreads(std::move(func));
			}
			size_t EnumWindows(std::function<bool(void*)> func) const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::None).EnumWindows(std::move(func));
			}
			uint32_t GetMainThread() const override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::None).GetMainThread();
			}

		public:
			SystemProcessInfo& operator=(const SystemProcessInfo&) = default;
			SystemProcessInfo& operator=(SystemProcessInfo&&) = default;
	};
}

namespace KxFramework::System
{
	size_t EnumRunningProcesses(std::function<bool(uint32_t)> func);
}
