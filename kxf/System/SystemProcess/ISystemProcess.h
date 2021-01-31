#pragma once
#include "../Common.h"
#include "kxf/General/String.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class KX_API ISystemProcess: public RTTI::Interface<ISystemProcess>
	{
		KxRTTI_DeclareIID(ISystemProcess, {0x3f3562cb, 0x5674, 0x4662, {0xa0, 0x45, 0x93, 0x28, 0x24, 0xfc, 0x33, 0x4c}});

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

			virtual SHWindowCommand GetShowWindowCommand() const = 0;
			virtual bool SetShowWindowCommand(SHWindowCommand value) = 0;

			virtual size_t EnumEnvironemntVariables(std::function<bool(const String&, const String&)> func) const = 0;

			virtual bool IsRunning() const = 0;
			virtual std::optional<uint32_t> GetExitCode() const = 0;
			virtual bool Terminate(uint32_t exitCode, bool force = false) = 0;

			virtual bool SuspendProcess() = 0;
			virtual bool ResumeProcess() = 0;

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
