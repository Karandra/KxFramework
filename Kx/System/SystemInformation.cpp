#include "KxStdAfx.h"
#include "SystemInformation.h"
#include "NativeAPI.h"
#include "Registry.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include <wx/settings.h>
#include <SDDL.h>

namespace
{
	bool CheckKernerlversion(const KxFramework::System::KernelVersion& kernelVersion)
	{
		// Major version should always be > 0, others are allowed to be zero.
		return kernelVersion.Major > 0 && kernelVersion.Minor >= 0 && kernelVersion.Build >= 0 && kernelVersion.ServicePackMajor >= 0 && kernelVersion.ServicePackMinor >= 0;
	}
	bool CheckVersionInfo(const KxFramework::System::VersionInfo& versionInfo)
	{
		using namespace KxFramework;

		return versionInfo.PlatformID != SystemPlatformID::Unknown &&
			versionInfo.SystemType != SystemType::Unknown &&
			versionInfo.ProductType != SystemProductType::Unknown &&
			versionInfo.ProductSuite != SystemProductSuite::None;
	}
}

namespace KxFramework::System
{
	bool Is64Bit() noexcept
	{
		#if defined(_WIN64)
		return true;
		#else
		BOOL isWow64 = FALSE;
		if (NativeAPI::Kernel32::IsWow64Process)
		{
			NativeAPI::Kernel32::IsWow64Process(::GetCurrentProcess(), &isWow64);
		}
		return isWow64 != FALSE;
		#endif
	}
	void GetRegistryQuota(BinarySize& used, BinarySize& allowed) noexcept
	{
		DWORD usedBytes = 0;
		DWORD allowedBytes = 0;
		if (::GetSystemRegistryQuota(&allowedBytes, &usedBytes))
		{
			used = BinarySize::FromBytes(usedBytes);
			allowed = BinarySize::FromBytes(allowedBytes);
		}
	}

	String GetProductName()
	{
		if (const auto versionInfo = GetVersionInfo())
		{
			return GetProductName(*versionInfo, Is64Bit());
		}
		return {};
	}
	String GetProductName(const VersionInfo& versionInfo, bool is64Bit)
	{
		// For reference: https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexa
		const int versionMajor = versionInfo.Kernel.Major;
		const int versionMinor = versionInfo.Kernel.Minor;
		const int buildNumber = versionInfo.Kernel.Build;
		const bool isHomeServer = versionInfo.ProductSuite & SystemProductSuite::HomeServer;
		const bool isWorkstation = versionInfo.SystemType == SystemType::Workstation;

		switch (versionMajor)
		{
			case 10:
			{
				switch (versionMinor)
				{
					case 0:
					{
						if (isWorkstation)
						{
							return "Windows 10";
						}
						else
						{
							// https://techcommunity.microsoft.com/t5/windows-server-insiders/windows-server-2019-version-info/m-p/234472
							if (buildNumber >= 17623)
							{
								return "Windows Server 2019";
							}
							else
							{
								return "Windows Server 2016";
							}
						}
					}
				};
				break;
			}
			case 6:
			{
				switch (versionMinor)
				{
					case 0:
					{
						if (isWorkstation)
						{
							return "Windows Vista";
						}
						else
						{
							return "Windows Server 2008";
						}
					}
					case 1:
					{
						if (isWorkstation)
						{
							return "Windows 7";
						}
						else
						{
							return "Windows Server 2008 R2";
						}
					}
					case 2:
					{
						if (isWorkstation)
						{
							return "Windows 8";
						}
						else
						{
							return "Windows Server 2012";
						}
					}
					case 3:
					{
						if (isWorkstation)
						{
							return "Windows 8.1";
						}
						else
						{
							return "Windows Server 2012 R2";
						}
					}
				};
				break;
			}
			case 5:
			{
				switch (versionMinor)
				{
					case 0:
					{
						return "Windows 2000";
					}
					case 1:
					{
						return "Windows XP";
					}
					case 2:
					{
						if (isWorkstation)
						{
							if (is64Bit)
							{
								return "Windows XP Professional x64 Edition";
							}
							else
							{
								// This branch shouldn't really be executes since there are no regular Windows XP with 6.2 kernel version
								return "Windows XP";
							}
						}
						else if (isHomeServer)
						{
							return "Windows Home Server";
						}
						else
						{
							return "Windows Server 2003";
						}
					}
				};
				break;
			}
		};
		return {};
	}

	std::optional<KernelVersion> GetKernelVersion() noexcept
	{
		if (NativeAPI::NtDLL::RtlGetVersion)
		{
			RTL_OSVERSIONINFOEXW infoEx = {};
			infoEx.dwOSVersionInfoSize = sizeof(infoEx);
			NativeAPI::NtDLL::RtlGetVersion(&infoEx);

			KernelVersion kernel(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.dwBuildNumber);
			kernel.ServicePackMajor = infoEx.wServicePackMajor;
			kernel.ServicePackMinor = infoEx.wServicePackMinor;

			if (CheckKernerlversion(kernel))
			{
				return kernel;
			}
		}
		return {};
	}
	std::optional<VersionInfo> GetVersionInfo() noexcept
	{
		if (NativeAPI::NtDLL::RtlGetVersion)
		{
			RTL_OSVERSIONINFOEXW infoEx = {};
			infoEx.dwOSVersionInfoSize = sizeof(infoEx);
			NativeAPI::NtDLL::RtlGetVersion(&infoEx);

			VersionInfo versionInfo(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.dwBuildNumber);
			versionInfo.Kernel.ServicePackMajor = infoEx.wServicePackMajor;
			versionInfo.Kernel.ServicePackMinor = infoEx.wServicePackMinor;

			if (CheckKernerlversion(versionInfo.Kernel))
			{
				versionInfo.PlatformID = FromInt<SystemPlatformID>(infoEx.dwPlatformId);
				versionInfo.SystemType = FromInt<SystemType>(infoEx.wProductType);
				versionInfo.ProductSuite = FromInt<SystemProductSuite>(infoEx.wSuiteMask);
				try
				{
					versionInfo.ServicePack = infoEx.szCSDVersion;
				}
				catch (...)
				{
					return {};
				}

				DWORD productType = 0;
				if (::GetProductInfo(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.wServicePackMajor, infoEx.wServicePackMinor, &productType))
				{
					versionInfo.ProductType = FromInt<SystemProductType>(productType);
				}

				if (CheckVersionInfo(versionInfo))
				{
					return versionInfo;
				}
			}
		}
		return {};
	}
	std::optional<MemoryStatus> GetGlobalMemoryStatus() noexcept
	{
		MEMORYSTATUSEX info = {};
		info.dwLength = sizeof(MEMORYSTATUSEX);
		if (::GlobalMemoryStatusEx(&info))
		{
			MemoryStatus memoryStatus;
			memoryStatus.TotalRAM = BinarySize::FromBytes(info.ullTotalPhys);
			memoryStatus.TotalVirtual = BinarySize::FromBytes(info.ullTotalVirtual);
			memoryStatus.TotalPageFile = BinarySize::FromBytes(info.ullTotalPageFile);
			memoryStatus.AvailableRAM = BinarySize::FromBytes(info.ullAvailPhys);
			memoryStatus.AvailableVirtual = BinarySize::FromBytes(info.ullAvailVirtual);
			memoryStatus.AvailablePageFile = BinarySize::FromBytes(info.ullAvailPageFile);
			memoryStatus.MemoryLoad = info.dwMemoryLoad / 100.0f;

			return memoryStatus;
		}
		return {};
	}
	BinarySize GetPhysicallyInstalledMemory() noexcept
	{
		ULONGLONG totalMemoryInKilobytes = 0;
		if (::GetPhysicallyInstalledSystemMemory(&totalMemoryInKilobytes))
		{
			return BinarySize::FromKB(totalMemoryInKilobytes);
		}
		return {};
	}
	
	std::optional<UserInfo> GetUserInfo()
	{
		UserInfo userInfo;

		// User name
		DWORD userNameLength = 0;
		::GetUserNameW(nullptr, &userNameLength);
		if (!::GetUserNameW(wxStringBuffer(userInfo.Name, userNameLength), &userNameLength))
		{
			return {};
		}

		// Organization
		RegistryKey key(RegistryBaseKey::LocalMachine, wxS("Software\\Microsoft\\Windows NT\\CurrentVersion"), RegistryAccess::Read);
		if (key)
		{
			if (auto value = key.GetStringValue(wxS("RegisteredOrganization")))
			{
				userInfo.Organization = std::move(*value);
			}
		}

		// Is administrator
		HANDLE tokenHandle = nullptr;
		Utility::CallAtScopeExit atExit([&]()
		{
			if (tokenHandle)
			{
				::CloseHandle(tokenHandle);
			}
		});

		if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
		{
			TOKEN_ELEVATION elevationInfo;
			DWORD size = sizeof(TOKEN_ELEVATION);
			if (::GetTokenInformation(tokenHandle, TokenElevation, &elevationInfo, sizeof(elevationInfo), &size))
			{
				userInfo.AdminRights = elevationInfo.TokenIsElevated;
			}

			TOKEN_ELEVATION_TYPE elevationType;
			size = sizeof(TOKEN_ELEVATION_TYPE);
			if (::GetTokenInformation(tokenHandle, TokenElevationType, &elevationType, sizeof(elevationType), &size))
			{
				userInfo.LimitedAdminRights = elevationType == TOKEN_ELEVATION_TYPE::TokenElevationTypeLimited;
			}
		}
		return userInfo;
	}
	String GetUserSID()
	{
		HANDLE tokenHandle = nullptr;
		wchar_t* userSID = nullptr;
		Utility::CallAtScopeExit atExit([&]()
		{
			if (tokenHandle)
			{
				::CloseHandle(tokenHandle);
			}
			if (userSID)
			{
				::LocalFree(userSID);
			}
		});

		if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
		{
			DWORD size = 0;
			::GetTokenInformation(tokenHandle, TokenUser, nullptr, 0, &size);

			std::vector<uint8_t> userToken(size, 0);
			if (::GetTokenInformation(tokenHandle, TokenUser, userToken.data(), size, &size))
			{
				::ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(userToken.data())->User.Sid, &userSID);
				return userSID;
			}
		}
		return {};
	}

	Color GetColor(wxSystemColour index) noexcept
	{
		return wxSystemSettings::GetColour(index);
	}
	int GetMetric(wxSystemMetric index, const wxWindow* window) noexcept
	{
		return wxSystemSettings::GetMetric(index, window);
	}
	size_t EnumStandardSounds(std::function<bool(String)> func)
	{
		RegistryKey key(RegistryBaseKey::CurrentUser, wxS("AppEvents\\EventLabels"), RegistryAccess::Read|RegistryAccess::Enumerate);
		if (key)
		{
			return key.EnumKeyNames(std::move(func));
		}
		return 0;
	}

	std::optional<DisplayInfo> GetDisplayInfo() noexcept
	{
		HDC desktopDC = ::GetDC(nullptr);
		if (desktopDC)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				::ReleaseDC(nullptr, desktopDC);
			});

			DisplayInfo displayInfo;
			displayInfo.Width = ::GetDeviceCaps(desktopDC, DESKTOPHORZRES);
			displayInfo.Height = ::GetDeviceCaps(desktopDC, DESKTOPVERTRES);
			displayInfo.Depth = ::GetDeviceCaps(desktopDC, BITSPIXEL);
			displayInfo.Frequency = ::GetDeviceCaps(desktopDC, VREFRESH);
			return displayInfo;
		}
		return {};
	}
	size_t EnumDisplayModes(std::function<bool(DisplayInfo)> func, const String& deviceName)
	{
		size_t count = 0;

		DEVMODEW deviceMode = {};
		deviceMode.dmSize = sizeof(deviceMode);
		while (::EnumDisplaySettingsW(deviceName.IsEmpty() ? nullptr : deviceName.wc_str(), count, &deviceMode))
		{
			DisplayInfo displayInfo;
			displayInfo.Width = deviceMode.dmPelsWidth;
			displayInfo.Height = deviceMode.dmPelsHeight;
			displayInfo.Depth = deviceMode.dmBitsPerPel;
			displayInfo.Frequency = deviceMode.dmDisplayFrequency;

			count++;
			if (!std::invoke(func, std::move(displayInfo)))
			{
				break;
			}
		}
		return count;
	}
	size_t EnumDisplayDevices(std::function<bool(DisplayDeviceInfo)> func)
	{
		bool isSuccess = false;
		DWORD index = 0;

		DISPLAY_DEVICE displayDevice = {};
		displayDevice.cb = sizeof(displayDevice);

		size_t count = 0;
		std::unordered_set<String> hash;
		do
		{
			isSuccess = ::EnumDisplayDevicesW(nullptr, index, &displayDevice, 0);
			if ((displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) || (displayDevice.StateFlags == 0 && std::wcscmp(displayDevice.DeviceString, L"") != 0))
			{
				if (hash.insert(displayDevice.DeviceString).second)
				{
					DisplayDeviceInfo deviceInfo;
					deviceInfo.DeviceName = displayDevice.DeviceName;
					deviceInfo.DeviceDescription = displayDevice.DeviceString;
					Utility::AddFlagRef(deviceInfo.Flags, DisplayDeviceFlag::Active, displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE);
					Utility::AddFlagRef(deviceInfo.Flags, DisplayDeviceFlag::Primary, displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE);
					Utility::AddFlagRef(deviceInfo.Flags, DisplayDeviceFlag::Removable, displayDevice.StateFlags & DISPLAY_DEVICE_REMOVABLE);
					Utility::AddFlagRef(deviceInfo.Flags, DisplayDeviceFlag::VGACompatible, displayDevice.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE);
					Utility::AddFlagRef(deviceInfo.Flags, DisplayDeviceFlag::MirroringDriver, displayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER);

					count++;
					if (!std::invoke(func, std::move(deviceInfo)))
					{
						break;
					}
				}
			}
			index++;
		}
		while (isSuccess);
		return count;
	}

	String GetEnvironmentVariable(const String& name)
	{
		DWORD length = ::GetEnvironmentVariableW(name.wc_str(), nullptr, 0);
		if (length != 0)
		{
			String result;
			::GetEnvironmentVariableW(name.wc_str(), wxStringBuffer(result, length), length);
			return result;
		}
		return {};
	}
	bool SetEnvironmentVariable(const String& name, const String& value)
	{
		return ::SetEnvironmentVariableW(name.wc_str(), value.wc_str());
	}
	size_t EnumEnvironmentVariables(std::function<bool(String, String)> func)
	{
		size_t count = 0;
		for (const wchar_t* item = ::GetEnvironmentStringsW(); *item; item += std::wcslen(item) + 1)
		{
			const wchar_t* separator = std::wcschr(item, L'=');
			if (separator && separator != item)
			{
				StringView name(item, separator - item);
				StringView value(separator + 1);

				count++;
				if (!std::invoke(func, String::FromView(name), String::FromView(value)))
				{
					break;
				}
			}
		}
		return count;
	}

	bool LockWorkstation(LockWorkstationCommand command) noexcept
	{
		switch (command)
		{
			case LockWorkstationCommand::LockScreen:
			{
				return ::LockWorkStation();
			}
			case LockWorkstationCommand::ScreenOff:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
			}
			case LockWorkstationCommand::ScreenOn:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
			}
			case LockWorkstationCommand::ScreenSleep:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);
			}
			case LockWorkstationCommand::ScreenWait:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)0);
			}
			case LockWorkstationCommand::ScreenSaver:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
			}
		};
		return false;
	}
	bool ExitWorkstation(ExitWorkstationCommand command) noexcept
	{
		UINT modeEWX = 0;
		Utility::AddFlagRef(modeEWX, EWX_LOGOFF, command & ExitWorkstationCommand::LogOff);
		Utility::AddFlagRef(modeEWX, EWX_POWEROFF, command & ExitWorkstationCommand::PowerOff);
		Utility::AddFlagRef(modeEWX, EWX_SHUTDOWN, command & ExitWorkstationCommand::Shutdown);
		Utility::AddFlagRef(modeEWX, EWX_REBOOT, command & ExitWorkstationCommand::Reboot);
		Utility::AddFlagRef(modeEWX, EWX_QUICKRESOLVE, command & ExitWorkstationCommand::QuickResolve);
		Utility::AddFlagRef(modeEWX, EWX_RESTARTAPPS, command & ExitWorkstationCommand::RestartApps);
		Utility::AddFlagRef(modeEWX, EWX_HYBRID_SHUTDOWN, command & ExitWorkstationCommand::HybridShutdown);
		Utility::AddFlagRef(modeEWX, EWX_BOOTOPTIONS, command & ExitWorkstationCommand::BootOptions);
		Utility::AddFlagRef(modeEWX, EWX_FORCE, command & ExitWorkstationCommand::Force);
		Utility::AddFlagRef(modeEWX, EWX_FORCEIFHUNG, command & ExitWorkstationCommand::ForceHung);

		return ::ExitWindowsEx(modeEWX, SHTDN_REASON_MAJOR_OTHER|SHTDN_REASON_MINOR_OTHER|SHTDN_REASON_FLAG_PLANNED);
	}

	bool IsWindowsServer() noexcept
	{
		auto version = GetVersionInfo();
		return version && version->SystemType == SystemType::Server;
	}
	bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor) noexcept
	{
		if (auto version = GetKernelVersion())
		{
			if (version->Major >= majorVersion && version->Minor >= minorVersion)
			{
				if (servicePackMajor != -1)
				{
					return version->ServicePackMajor >= servicePackMajor;
				}
				return true;
			}
		}
		return false;
	}
}

namespace KxFramework::System
{
	DisableFSRedirection::DisableFSRedirection() noexcept
	{
		if (NativeAPI::Kernel32::Wow64DisableWow64FsRedirection)
		{
			NativeAPI::Kernel32::Wow64DisableWow64FsRedirection(&m_Value);
		}
	}
	DisableFSRedirection::~DisableFSRedirection() noexcept
	{
		if (NativeAPI::Kernel32::Wow64RevertWow64FsRedirection && m_Value)
		{
			NativeAPI::Kernel32::Wow64RevertWow64FsRedirection(&m_Value);
		}
	}
}
