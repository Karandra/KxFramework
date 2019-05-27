/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWinUndef.h"

enum KxSystemPowerState
{
	PSX_HYBRID_SHUTDOWN = EWX_HYBRID_SHUTDOWN,
	PSX_LOGOFF = EWX_LOGOFF,
	PSX_POWEROFF = EWX_POWEROFF,
	PSX_REBOOT = EWX_REBOOT,
	PSX_RESTARTAPPS = EWX_RESTARTAPPS,
	PSX_SHUTDOWN = EWX_SHUTDOWN,
	PSX_BOOTOPTIONS = EWX_BOOTOPTIONS,

	PSX_LOCKSCREEN = -1,
	PSX_SCREENON = -2,
	PSX_SCREENOFF = -3,
	PSX_SCREENSLEEP = -4,
	PSX_SCREENWAIT = -5,
	PSX_SCREENSAVER = -6
};

class KX_API KxProcess;
class KX_API KxSystemFSRedirector;
class KX_API KxSystem
{
	friend class KxSystemFSRedirector;
	friend class KxProcess;

	public:
		struct KernelVersion
		{
			public:
				const int Major = -1;
				const int Minor = -1;
				const int Build = -1;

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
					return Major > 0 && Minor >= 0 && Build >= 0;
				}
		};
		struct VersionInfo
		{
			public:
				KernelVersion Kernel;

				wxString ServicePack;
				int PlatformID = -1;
				int ServicePackMajor = -1;
				int ServicePackMinor = -1;
				int ProductType = -1;
				int ProductSuite = -1;

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
						PlatformID >= 0 &&
						ServicePackMajor >= 0 &&
						ServicePackMinor >= 0 &&
						ProductType >= 0 &&
						ProductSuite >= 0;
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

	public:
		static wxString FormatMessage(DWORD flags, const void* source, DWORD messageID, DWORD langID = 0);

		static bool Is64Bit();
		static wxString GetName();
		static KernelVersion GetKernelVersion();
		static bool IsKeyDown(int keyCode);
		static DWORD GetProductInfo();
		static VersionInfo GetVersionInfo();
		static MemoryInfo GetMemoryInfo();
		static bool SetPowerState(KxSystemPowerState mode, bool force = false, bool forceHung = false);
		static wxString GetUserSID();
		static UserInfo GetUserInfo();
		static DWORD GetLastError();
		static wxString GetErrorMessage(DWORD messageID, DWORD langID = 0);

		static wxString GetEnvironmentVariable(const wxString& name);
		static bool SetEnvironmentVariable(const wxString& name, const wxString& value);
		static KxStringToStringUMap GetEnvironmentVariables();

		static bool IsWindowsServer();
		static bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor = -1);
		static bool IsWindows10OrGreater()
		{
			return IsWindowsVersionOrGreater(10, 0);
		}
		static bool IsWindows8Point1OrGreater()
		{
			return IsWindowsVersionOrGreater(6, 3);
		}
		static bool IsWindows8OrGreater()
		{
			return IsWindowsVersionOrGreater(6, 2);
		}
		static bool IsWindows7SP1OrGreater()
		{
			return IsWindowsVersionOrGreater(6, 1, 1);
		}
		static bool IsWindows7OrGreater()
		{
			return IsWindowsVersionOrGreater(6, 1);
		}
		static bool IsWindowsVistaSP2OrGreater()
		{
			return IsWindowsVersionOrGreater(6, 0, 2);
		}
		static bool IsWindowsVistaSP1OrGreater()
		{
			return IsWindowsVersionOrGreater(6, 0, 1);
		}
		static bool IsWindowsVistaOrGreater()
		{
			return IsWindowsVersionOrGreater(6, 0);
		}
		static bool IsWindowsXPSP3OrGreater()
		{
			return IsWindowsVersionOrGreater(6, 1, 3);
		}
		static bool IsWindowsXPSP2OrGreater()
		{
			return IsWindowsVersionOrGreater(5, 1, 2);
		}
		static bool IsWindowsXPSP1OrGreater()
		{
			return IsWindowsVersionOrGreater(5, 1, 1);
		}
		static bool IsWindowsXPOrGreater()
		{
			return IsWindowsVersionOrGreater(5, 1);
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxSystemFSRedirector
{
	private:
		void* m_Value = nullptr;

	public:
		KxSystemFSRedirector();
		~KxSystemFSRedirector();
};
