/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWinUndef.h"
#include "KxFramework/KxSystemEnums.h"
class KX_API KxProcess;
class KX_API KxSystemFSRedirector;

namespace KxSystem
{
	struct KernelVersion
	{
		public:
			int Major = -1;
			int Minor = -1;
			int Build = -1;

			int ServicePackMajor = -1;
			int ServicePackMinor = -1;

		public:
			KernelVersion() = default;
			KernelVersion(int major, int minor = 0, int build = 0)
				:Major(major), Minor(minor), Build(build)
			{
			}

		public:
			bool IsOK() const
			{
				// Major version should be > 0, others are allowed to be zero
				return Major > 0 && Minor >= 0 && Build >= 0 && ServicePackMajor >= 0 && ServicePackMinor >= 0;
			}
	};
	struct VersionInfo
	{
		public:
			KernelVersion Kernel;

			wxString ServicePack;
			PlatformID PlatformID = PlatformID::Unknown;
			ProductSuite ProductSuite = ProductSuite::None;
			SystemType SystemType = SystemType::Unknown;
			ProductType ProductType = ProductType::Unknown;

		public:
			VersionInfo() = default;
			VersionInfo(int major, int minor = 0, int build = 0)
				:Kernel(major, minor, build)
			{
			}

		public:
			bool IsOK() const
			{
				return Kernel.IsOK() &&
					PlatformID != PlatformID::Unknown &&
					SystemType != SystemType::Unknown &&
					ProductType != ProductType::Unknown &&
					ProductSuite != ProductSuite::None;
			}
			bool HasServicePack() const
			{
				return !ServicePack.IsEmpty();
			}
	};

	struct MemoryInfo
	{
		int MemoryLoad = 0;
		uint64_t TotalRAM = 0;
		uint64_t TotalVirtual = 0;
		uint64_t TotalPageFile = 0;
		uint64_t AvailableRAM = 0;
		uint64_t AvailableVirtual = 0;
		uint64_t AvailablePageFile = 0;
	};
	struct UserInfo
	{
		wxString Name;
		wxString Organization;
		bool Admin = false;
		bool AdminLimited = false;
	};
}

namespace KxSystem
{
	uint32_t GetLastError();
	wxString FormatMessage(uint32_t flags, const void* source, uint32_t messageID, uint32_t langID = 0);
	wxString GetErrorMessage(uint32_t messageID, uint32_t langID = 0);
	wxString GetLastErrorMessage(uint32_t langID = 0);

	bool Is64Bit();
	wxString GetName();

	KernelVersion GetKernelVersion();
	VersionInfo GetVersionInfo();
	MemoryInfo GetMemoryInfo();
	UserInfo GetUserInfo();
	wxString GetUserSID();

	wxString GetEnvironmentVariable(const wxString& name);
	bool SetEnvironmentVariable(const wxString& name, const wxString& value);
	KxStringToStringUMap GetEnvironmentVariables();

	bool ExitWorkstation(ExitCommand command, bool force = false, bool forceHung = false);
	bool LockWorkstation(LockCommand command);

	bool IsWindowsServer();
	bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor = -1);
	inline bool IsWindows10OrGreater()
	{
		return IsWindowsVersionOrGreater(10, 0);
	}
	inline bool IsWindows8Point1OrGreater()
	{
		return IsWindowsVersionOrGreater(6, 3);
	}
	inline bool IsWindows8OrGreater()
	{
		return IsWindowsVersionOrGreater(6, 2);
	}
	inline bool IsWindows7SP1OrGreater()
	{
		return IsWindowsVersionOrGreater(6, 1, 1);
	}
	inline bool IsWindows7OrGreater()
	{
		return IsWindowsVersionOrGreater(6, 1);
	}
	inline bool IsWindowsVistaSP2OrGreater()
	{
		return IsWindowsVersionOrGreater(6, 0, 2);
	}
	inline bool IsWindowsVistaSP1OrGreater()
	{
		return IsWindowsVersionOrGreater(6, 0, 1);
	}
	inline bool IsWindowsVistaOrGreater()
	{
		return IsWindowsVersionOrGreater(6, 0);
	}
	inline bool IsWindowsXPSP3OrGreater()
	{
		return IsWindowsVersionOrGreater(6, 1, 3);
	}
	inline bool IsWindowsXPSP2OrGreater()
	{
		return IsWindowsVersionOrGreater(5, 1, 2);
	}
	inline bool IsWindowsXPSP1OrGreater()
	{
		return IsWindowsVersionOrGreater(5, 1, 1);
	}
	inline bool IsWindowsXPOrGreater()
	{
		return IsWindowsVersionOrGreater(5, 1);
	}
};

namespace KxSystem
{
	class KX_API FSRedirector final
	{
		private:
			void* m_Value = nullptr;

		public:
			FSRedirector();
			~FSRedirector();
	};
}