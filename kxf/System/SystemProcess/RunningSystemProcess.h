#pragma once
#include "ISystemProcess.h"

namespace kxf
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

			SHWindowCommand GetShowWindowCommand() const override
			{
				return SHWindowCommand::None;
			}
			bool SetShowWindowCommand(SHWindowCommand value) override
			{
				return false;
			}

			size_t EnumEnvironemntVariables(std::function<bool(const String&, const String&)> func) const override
			{
				return 0;
			}

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

			bool SuspendProcess() override;
			bool ResumeProcess() override;

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