#pragma once
#include "Common.h"
#include "SystemInformationDefines.h"
#include "UserName.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Version.h"
#include "kxf/Core/BinarySize.h"
#include "kxf/Core/DateTime.h"
#include "kxf/Core/LocallyUniqueID.h"
#include "kxf/Core/Enumerator.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/Drawing/Color.h"
#include "kxf/Drawing/Font.h"
#include "UndefWindows.h"

namespace kxf::System
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
			FlagSet<SystemProductSuite> ProductSuite;

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
		UserName Name;
		String Organization;
		bool AdminRights = false;
		bool LimitedAdminRights = false;
	};
	struct EnvironmentVariable final
	{
		String Name;
		String Value;
	};

	struct DisplayInfo final
	{
		int Width = 0;
		int Height = 0;
		int BitDepth = 0;
		int RefreshRate = 0;
	};
	struct DisplayDeviceInfo final
	{
		String DeviceName;
		String DeviceDescription;
		FlagSet<DisplayDeviceFlag> Flags;
	};
	struct DisplayAdapterInfo final
	{
		String Name;
		uint32_t Index = 0;
		uint32_t VendorID = 0;
		uint32_t DeviceID = 0;
		uint32_t SubSystemID = 0;
		uint32_t Revision = 0;
		LocallyUniqueID UniqueID;
		FlagSet<DisplayAdapterFlag> Flags;
		BinarySize DedicatedVideoMemory;
		BinarySize DedicatedSystemMemory;
		BinarySize SharedSystemMemory;
	};
}

namespace kxf::System
{
	KX_API bool Is64Bit() noexcept;
	KX_API void GetRegistryQuota(BinarySize& used, BinarySize& allowed) noexcept;

	KX_API String GetProductName();
	KX_API String GetProductName(const VersionInfo& versionInfo, bool is64Bit);

	KX_API std::optional<KernelVersion> GetKernelVersion() noexcept;
	KX_API std::optional<VersionInfo> GetVersionInfo() noexcept;
	KX_API std::optional<MemoryStatus> GetGlobalMemoryStatus() noexcept;
	KX_API BinarySize GetPhysicallyInstalledMemory() noexcept;

	KX_API std::optional<UserInfo> GetUserInfo();
	KX_API String GetUserSID();

	KX_API Color GetColor(SystemColor index) noexcept;
	KX_API Font GetFont(SystemFont index);
	KX_API SystemScreenType GetScreenType() noexcept;
	KX_API int GetMetric(SystemMetric index, const wxWindow* window = nullptr) noexcept;
	KX_API Size GetMetric(SystemSizeMetric index, const wxWindow* window = nullptr) noexcept;
	KX_API TimeSpan GetMetric(SystemTimeMetric index, const wxWindow* window = nullptr) noexcept;
	KX_API bool HasFeature(SystemFeature feature) noexcept;
	KX_API Enumerator<String> EnumStandardSounds();

	KX_API std::optional<DisplayInfo> GetDisplayInfo() noexcept;
	KX_API Enumerator<DisplayInfo> EnumDisplayModes(const String& deviceName = {});
	KX_API Enumerator<DisplayDeviceInfo> EnumDisplayDevices(const String& deviceName = {});
	KX_API Enumerator<DisplayAdapterInfo> EnumDisplayAdapters();

	KX_API String ExpandEnvironmentStrings(const String& strings);
	KX_API String GetEnvironmentVariable(const String& name);
	KX_API bool SetEnvironmentVariable(const String& name, const String& value);
	KX_API Enumerator<EnvironmentVariable> EnumEnvironmentVariables();

	KX_API bool LockWorkstation(LockWorkstationCommand command) noexcept;
	KX_API bool ExitWorkstation(FlagSet<ExitWorkstationCommand> command) noexcept;

	KX_API bool IsWindowsServer() noexcept;
	KX_API bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor = -1) noexcept;
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
