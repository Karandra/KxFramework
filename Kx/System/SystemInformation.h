#pragma once
#include "Common.h"
#include "ErrorCodeValue.h"
#include "Kx/General/String.h"
#include "Kx/General/Version.h"
#include "Kx/General/BinarySize.h"
#include "SystemInformationDefines.h"
#include "UndefWindows.h"

namespace KxFramework::System
{
	struct KernelVersion final
	{
		public:
			int Major = -1;
			int Minor = -1;
			int Build = -1;

			int ServicePackMajor = -1;
			int ServicePackMinor = -1;

		public:
			KernelVersion() noexcept = default;
			KernelVersion(int major, int minor = 0, int build = 0) noexcept
				:Major(major), Minor(minor), Build(build)
			{
			}

		public:
			operator Version() const noexcept
			{
				return {Major, Minor, Build, ServicePackMajor, ServicePackMinor};
			}
	};
	struct VersionInfo final
	{
		public:
			KernelVersion Kernel;

			String ServicePack;
			SystemType SystemType = SystemType::Unknown;
			SystemPlatformID PlatformID = SystemPlatformID::Unknown;
			SystemProductType ProductType = SystemProductType::Unknown;
			SystemProductSuite ProductSuite = SystemProductSuite::None;

		public:
			VersionInfo() noexcept = default;
			VersionInfo(int major, int minor = 0, int build = 0) noexcept
				:Kernel(major, minor, build)
			{
			}
	};

	struct MemoryStatus final
	{
		BinarySize TotalRAM;
		BinarySize TotalVirtual;
		BinarySize TotalPageFile;
		BinarySize AvailableRAM;
		BinarySize AvailableVirtual;
		BinarySize AvailablePageFile;
		float MemoryLoad = 0.0f;
	};
	struct UserInfo final
	{
		String Name;
		String Organization;
		bool AdminRights = false;
		bool LimitedAdminRights = false;
	};
}

namespace KxFramework::System
{
	bool Is64Bit() noexcept;

	String GetProductName();
	String GetProductName(const VersionInfo& versionInfo, bool is64Bit);

	std::optional<KernelVersion> GetKernelVersion() noexcept;
	std::optional<VersionInfo> GetVersionInfo() noexcept;
	std::optional<MemoryStatus> GetGlobalMemoryStatus() noexcept;
	BinarySize GetPhysicallyInstalledMemory() noexcept;
	std::optional<UserInfo> GetUserInfo();
	String GetUserSID();

	String GetEnvironmentVariable(const String& name);
	bool SetEnvironmentVariable(const String& name, const String& value);
	size_t EnumEnvironmentVariables(std::function<bool(String, String)> func);

	bool LockWorkstation(LockWorkstationCommand command) noexcept;
	bool ExitWorkstation(ExitWorkstationCommand command) noexcept;

	bool IsWindowsServer() noexcept;
	bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor = -1) noexcept;
	inline bool IsWindows10OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(10, 0);
	}
	inline bool IsWindows8Point1OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(6, 3);
	}
	inline bool IsWindows8OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(6, 2);
	}
	inline bool IsWindows7SP1OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(6, 1, 1);
	}
	inline bool IsWindows7OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(6, 1);
	}
	inline bool IsWindowsVistaSP2OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(6, 0, 2);
	}
	inline bool IsWindowsVistaSP1OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(6, 0, 1);
	}
	inline bool IsWindowsVistaOrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(6, 0);
	}
	inline bool IsWindowsXPSP3OrGreater()
	{
		return IsWindowsVersionOrGreater(5, 1, 3);
	}
	inline bool IsWindowsXPSP2OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(5, 1, 2);
	}
	inline bool IsWindowsXPSP1OrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(5, 1, 1);
	}
	inline bool IsWindowsXPOrGreater() noexcept
	{
		return IsWindowsVersionOrGreater(5, 1);
	}
};

namespace KxFramework::System
{
	class KX_API DisableFSRedirection final
	{
		private:
			void* m_Value = nullptr;

		public:
			DisableFSRedirection() noexcept;
			~DisableFSRedirection() noexcept;
	};
}