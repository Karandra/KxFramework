#pragma once
#include "ISystemProcess.h"
#include "RunningSystemProcess.h"
#include <unordered_map>

namespace kxf
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
			SystemProcessPriority m_Priority = SystemProcessPriority::None;
			SHWindowCommand m_ShowWindowCommand = SHWindowCommand::None;

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
				return m_Priority;
			}
			bool SetPriority(SystemProcessPriority value) override
			{
				m_Priority = value;
				return true;
			}

			SHWindowCommand GetShowWindowCommand() const override
			{
				return m_ShowWindowCommand;
			}
			bool SetShowWindowCommand(SHWindowCommand value) override
			{
				m_ShowWindowCommand = value;
				return true;
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

			bool SuspendProcess() override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::SuspendResume).SuspendProcess();
			}
			bool ResumeProcess() override
			{
				return RunningSystemProcess(m_PID, SystemProcessAccess::SuspendResume).ResumeProcess();
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

			size_t EnumEnvironemntVariables(std::function<bool(const String&, const String&)> func) const override
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
