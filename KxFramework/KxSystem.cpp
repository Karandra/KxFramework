/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxSystem.h"
#include "KxFramework/KxSystemAPI.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxRegistry.h"
#include "KxFramework/KxUtility.h"
#include <SDDL.h>

namespace KxSystem
{
	using namespace KxEnumClassOperations;

	uint32_t GetLastError()
	{
		return ::GetLastError();
	}
	wxString FormatMessage(uint32_t flags, const void* source, uint32_t messageID, uint32_t langID)
	{
		wchar_t* formattedMessage = nullptr;
		uint32_t length = ::FormatMessageW(flags|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,
										   source,
										   messageID,
										   MAKELCID(langID, SORT_DEFAULT),
										   reinterpret_cast<LPWSTR>(&formattedMessage),
										   0,
										   nullptr
		);
		if (length != 0 && formattedMessage)
		{
			wxString message(formattedMessage, length);
			::LocalFree(formattedMessage);
			return message;
		}
		return wxEmptyString;
	}
	wxString GetErrorMessage(uint32_t messageID, uint32_t langID)
	{
		return FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, messageID, langID);
	}
	wxString GetLastErrorMessage(uint32_t langID)
	{
		return GetErrorMessage(::GetLastError(), langID);
	}

	bool Is64Bit()
	{
		#if defined(_WIN64)
		return true;
		#else
		BOOL isWow64 = FALSE;
		if (KxSystemAPI::IsWow64Process)
		{
			KxSystemAPI::IsWow64Process(::GetCurrentProcess(), &isWow64);
		}
		return isWow64 != FALSE;
		#endif
	}
	wxString GetName()
	{
		if (!KxSystemAPI::RtlGetVersion)
		{
			return wxEmptyString;
		}

		// For reference: https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-osversioninfoexa
		const VersionInfo info = GetVersionInfo();
		const int versionMajor = info.Kernel.Major;
		const int versionMinor = info.Kernel.Minor;
		const bool isHomeServer = info.ProductSuite & ProductSuite::HomeServer;
		const bool isWorkstation = info.SystemType == SystemType::Workstation;
		
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
							return "Windows Server 2016";
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
							if (Is64Bit())
							{
								return "Windows XP Professional x64 Edition";
							}
							else
							{
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
		return wxEmptyString;
	}

	KernelVersion GetKernelVersion()
	{
		if (KxSystemAPI::RtlGetVersion)
		{
			RTL_OSVERSIONINFOEXW infoEx = {};
			infoEx.dwOSVersionInfoSize = sizeof(infoEx);
			KxSystemAPI::RtlGetVersion(&infoEx);

			KernelVersion kernel(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.dwBuildNumber);
			kernel.ServicePackMajor = infoEx.wServicePackMajor;
			kernel.ServicePackMinor = infoEx.wServicePackMinor;

			return kernel;
		}
		return {};
	}
	VersionInfo GetVersionInfo()
	{
		if (KxSystemAPI::RtlGetVersion)
		{
			RTL_OSVERSIONINFOEXW infoEx = {};
			infoEx.dwOSVersionInfoSize = sizeof(infoEx);
			KxSystemAPI::RtlGetVersion(&infoEx);

			VersionInfo versionInfo(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.dwBuildNumber);
			versionInfo.Kernel.ServicePackMajor = infoEx.wServicePackMajor;
			versionInfo.Kernel.ServicePackMinor = infoEx.wServicePackMinor;

			versionInfo.PlatformID = FromInt<PlatformID>(infoEx.dwPlatformId);
			versionInfo.SystemType = FromInt<SystemType>(infoEx.wProductType);
			versionInfo.ProductSuite = FromInt<ProductSuite>(infoEx.wSuiteMask);
			versionInfo.ServicePack = infoEx.szCSDVersion;

			DWORD productType = 0;
			if (::GetProductInfo(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.wServicePackMajor, infoEx.wServicePackMinor, &productType))
			{
				versionInfo.ProductType = FromInt<ProductType>(productType);
			}

			return versionInfo;
		}
		return {};
	}
	MemoryInfo GetMemoryInfo()
	{
		MEMORYSTATUSEX info = {};
		info.dwLength = sizeof(MEMORYSTATUSEX);
		::GlobalMemoryStatusEx(&info);

		MemoryInfo memoryInfo;
		memoryInfo.MemoryLoad = info.dwMemoryLoad;
		memoryInfo.TotalRAM = info.ullTotalPhys;
		memoryInfo.TotalVirtual = info.ullTotalVirtual;
		memoryInfo.TotalPageFile = info.ullTotalPageFile;
		memoryInfo.AvailableRAM = info.ullAvailPhys;
		memoryInfo.AvailableVirtual = info.ullAvailVirtual;
		memoryInfo.AvailablePageFile = info.ullAvailPageFile;

		return memoryInfo;
	}
	UserInfo GetUserInfo()
	{
		UserInfo userInfo;

		// User name
		DWORD userNameLength = 0;
		::GetUserNameW(nullptr, &userNameLength);
		::GetUserNameW(wxStringBuffer(userInfo.Name, userNameLength), &userNameLength);

		// Organization
		userInfo.Organization = KxRegistry::GetValue(KxREG_HKEY_LOCAL_MACHINE, R"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)", "RegisteredOrganization", KxREG_VALUE_SZ).As<wxString>();

		// Is admin
		HANDLE tokenHandle = nullptr;
		if (::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
		{
			TOKEN_ELEVATION elevationInfo;
			DWORD size = sizeof(TOKEN_ELEVATION);
			if (GetTokenInformation(tokenHandle, TokenElevation, &elevationInfo, sizeof(elevationInfo), &size))
			{
				userInfo.Admin = elevationInfo.TokenIsElevated;
			}

			TOKEN_ELEVATION_TYPE elevationType;
			size = sizeof(TOKEN_ELEVATION_TYPE);
			if (::GetTokenInformation(tokenHandle, TokenElevationType, &elevationType, sizeof(elevationType), &size))
			{
				userInfo.AdminLimited = elevationType == TOKEN_ELEVATION_TYPE::TokenElevationTypeLimited;
			}
		}

		if (tokenHandle)
		{
			::CloseHandle(tokenHandle);
		}
		return userInfo;
	}
	wxString GetUserSID()
	{
		HANDLE tokenHandle = nullptr;
		wchar_t* userSID = nullptr;
		if (::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
		{
			DWORD size = 0;
			::GetTokenInformation(tokenHandle, TokenUser, nullptr, 0, &size);

			std::vector<uint8_t> userToken(size, 0);
			if (::GetTokenInformation(tokenHandle, TokenUser, userToken.data(), size, &size))
			{
				::ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(userToken.data())->User.Sid, &userSID);
			}
		}
		if (tokenHandle)
		{
			::CloseHandle(tokenHandle);
		}

		
		if (wxString stringSID; userSID)
		{
			stringSID = userSID;
			::LocalFree(userSID);
			return stringSID;
		}
		return wxEmptyString;
	}

	wxString GetEnvironmentVariable(const wxString& name)
	{
		DWORD length = ::GetEnvironmentVariableW(name.wc_str(), nullptr, 0);
		if (length != 0)
		{
			wxString out;
			::GetEnvironmentVariableW(name.wc_str(), wxStringBuffer(out, length), length);
			return out;
		}
		return wxEmptyString;
	}
	bool SetEnvironmentVariable(const wxString& name, const wxString& value)
	{
		return ::SetEnvironmentVariableW(name.wc_str(), value.wc_str());
	}
	KxStringToStringUMap GetEnvironmentVariables()
	{
		KxStringToStringUMap map;
		for (const wchar_t* item = ::GetEnvironmentStringsW(); *item; item += wcslen(item) + 1)
		{
			const wchar_t* separator = wcschr(item, L'=');
			map.insert_or_assign(wxString(item, separator - item), wxString(separator + 1));
		}
		return map;
	}

	bool ExitWorkstation(ExitCommand command, bool force, bool forceHung)
	{
		UINT modeEWX = ToInt(command);
		if (force)
		{
			modeEWX |= EWX_FORCE;
		}
		if (forceHung)
		{
			modeEWX |= EWX_FORCEIFHUNG;
		}
		return ::ExitWindowsEx(modeEWX, SHTDN_REASON_MAJOR_OTHER|SHTDN_REASON_MINOR_OTHER|SHTDN_REASON_FLAG_PLANNED);
	}
	bool LockWorkstation(LockCommand command)
	{
		switch (command)
		{
			case LockCommand::LockScreen:
			{
				return ::LockWorkStation();
			}
			case LockCommand::ScreenOff:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
			}
			case LockCommand::ScreenOn:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
			}
			case LockCommand::ScreenSleep:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);
			}
			case LockCommand::ScreenWait:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)0);
			}
			case LockCommand::ScreenSaver:
			{
				return !::SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
			}
		};
		return false;
	}

	bool IsWindowsServer()
	{
		return GetVersionInfo().SystemType == SystemType::Server;
	}
	bool IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor)
	{
		if (KernelVersion version = GetKernelVersion(); version.IsOK())
		{
			if (version.Major >= majorVersion && version.Minor >= minorVersion)
			{
				if (servicePackMajor != -1)
				{
					return version.ServicePackMajor >= servicePackMajor;
				}
				return true;
			}
		}
		return false;
	}
}

namespace KxSystem
{
	FSRedirector::FSRedirector()
	{
		if (KxSystemAPI::Wow64DisableWow64FsRedirection)
		{
			KxSystemAPI::Wow64DisableWow64FsRedirection(&m_Value);
		}
	}
	FSRedirector::~FSRedirector()
	{
		if (KxSystemAPI::Wow64RevertWow64FsRedirection && m_Value)
		{
			KxSystemAPI::Wow64RevertWow64FsRedirection(&m_Value);
			m_Value = nullptr;
		}
	}
}
