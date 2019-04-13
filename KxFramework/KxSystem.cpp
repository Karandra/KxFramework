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

wxString KxSystem::FormatMessage(DWORD flags, const void* source, DWORD messageID, DWORD langID)
{
	LPWSTR formattedMessage = nullptr;
	DWORD length = ::FormatMessageW(flags|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS, source, messageID, MAKELCID(langID, SORT_DEFAULT), (LPWSTR)&formattedMessage, 0, nullptr);
	if (length != 0 && formattedMessage)
	{
		wxString message(formattedMessage, length);
		LocalFree(formattedMessage);
		return message;
	}
	return wxEmptyString;
}

bool KxSystem::Is64Bit()
{
	#if defined(_WIN64)
	return true;
	#else
	BOOL bIsWow64 = FALSE;
	if (KxSystemAPI::IsWow64Process)
	{
		KxSystemAPI::IsWow64Process(::GetCurrentProcess(), &bIsWow64);
	}
	return bIsWow64 != FALSE;
	#endif
}
KxSystem::KernelVersion KxSystem::GetKernelVersion()
{
	RTL_OSVERSIONINFOEXW osVersionInfo = {0};
	osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
	KxSystemAPI::RtlGetVersion(&osVersionInfo);

	return KernelVersion(osVersionInfo.dwMajorVersion, osVersionInfo.dwMinorVersion, osVersionInfo.dwBuildNumber);
}
wxString KxSystem::GetName()
{
	RTL_OSVERSIONINFOEXW info = {0};
	info.dwOSVersionInfoSize = sizeof(info);
	KxSystemAPI::RtlGetVersion(&info);
	//
	DWORD versionMajor = info.dwMajorVersion;
	DWORD versionMinor = info.dwMinorVersion;
	WORD productType = info.wProductType;
	bool isHomeServer = info.wSuiteMask & VER_SUITE_WH_SERVER;
	bool isWorkstation = productType == VER_NT_WORKSTATION;
	//
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
			}
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
					return "Windows 8.1";
				}
			};
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
						return "Windows XP Professional x64 Edition";
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
bool KxSystem::IsKeyDown(int keyCode)
{
	return (1 << 16) & GetAsyncKeyState(keyCode);
}
DWORD KxSystem::GetProductInfo()
{
	RTL_OSVERSIONINFOEXW info = {0};
	info.dwOSVersionInfoSize = sizeof(info);
	KxSystemAPI::RtlGetVersion(&info);
	//
	DWORD productType = 0;
	::GetProductInfo(info.dwMajorVersion, info.dwMinorVersion, info.wServicePackMajor, info.wServicePackMinor, &productType);
	return productType;
}
KxSystem::VersionInfo KxSystem::GetVersionInfo()
{
	RTL_OSVERSIONINFOEXW osVersionInfo = {0};
	osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
	KxSystemAPI::RtlGetVersion(&osVersionInfo);

	VersionInfo versionInfo(osVersionInfo.dwMajorVersion, osVersionInfo.dwMinorVersion, osVersionInfo.dwBuildNumber);
	versionInfo.PlatformID = osVersionInfo.dwPlatformId;
	versionInfo.ServicePack = osVersionInfo.szCSDVersion;
	versionInfo.ServicePackMajor = osVersionInfo.wServicePackMajor;
	versionInfo.ServicePackMinor = osVersionInfo.wServicePackMinor;
	versionInfo.ProductType = osVersionInfo.wProductType;
	versionInfo.ProductSuite = osVersionInfo.wSuiteMask;
	return versionInfo;
}
KxSystem::MemoryInfo KxSystem::GetMemoryInfo()
{
	MEMORYSTATUSEX info = {0};
	info.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&info);

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
bool KxSystem::SetPowerState(KxSystemPowerState mode, bool force, bool forceHung)
{
	BOOL ret = FALSE;
	switch (mode)
	{
		case PSX_LOCKSCREEN:
		{
			ret = LockWorkStation();
			break;
		}
		case PSX_SCREENOFF:
		{
			ret = !SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
			break;
		}
		case PSX_SCREENON:
		{
			ret = !SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
			break;
		}
		case PSX_SCREENSLEEP:
		{
			ret = !SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);
			break;
		}
		case PSX_SCREENWAIT:
		{
			ret = !SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)0);
			break;
		}
		case PSX_SCREENSAVER:
		{
			ret = !SendMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
			break;
		}
		default:
		{
			UINT modeEWX = mode;
			if (force)
			{
				modeEWX |= EWX_FORCE;
			}
			if (forceHung)
			{
				modeEWX |= EWX_FORCEIFHUNG;
			}
			ret = ExitWindowsEx(modeEWX, SHTDN_REASON_MAJOR_OTHER|SHTDN_REASON_MINOR_OTHER|SHTDN_REASON_FLAG_PLANNED);
		}
	};
	return ret;
}
wxString KxSystem::GetUserSID()
{
	HANDLE tokenHandle = nullptr;
	LPWSTR sid = nullptr;
	if (::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
	{
		DWORD size = 0;
		::GetTokenInformation(tokenHandle, TokenUser, nullptr, 0, &size);

		TOKEN_USER* userToken = (TOKEN_USER*)malloc(size);
		if (userToken)
		{
			memset(userToken, 0, size);
			if (::GetTokenInformation(tokenHandle, TokenUser, userToken, size, &size))
			{
				::ConvertSidToStringSidW(userToken->User.Sid, &sid);
			}
			free(userToken);
		}
	}
	if (tokenHandle)
	{
		::CloseHandle(tokenHandle);
	}

	wxString out;
	if (sid != nullptr)
	{
		out = sid;
		::LocalFree(sid);
	}
	return out;
}
KxSystem::UserInfo KxSystem::GetUserInfo()
{
	UserInfo userInfo;

	// User name
	DWORD userNameLength = 0;
	GetUserNameW(nullptr, &userNameLength);
	GetUserNameW(wxStringBuffer(userInfo.Name, userNameLength), &userNameLength);

	// Organization
	userInfo.Organization = KxRegistry::GetValue(KxREG_HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "RegisteredOrganization", KxREG_VALUE_SZ).As<wxString>();

	// Is admin
	HANDLE tokenHandle = nullptr;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle))
	{
		TOKEN_ELEVATION elevationInfo;
		DWORD size = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(tokenHandle, TokenElevation, &elevationInfo, sizeof(elevationInfo), &size))
		{
			userInfo.Admin = elevationInfo.TokenIsElevated;
		}

		TOKEN_ELEVATION_TYPE elevationType;
		size = sizeof(TOKEN_ELEVATION_TYPE);
		if (GetTokenInformation(tokenHandle, TokenElevationType, &elevationType, sizeof(elevationType), &size))
		{
			userInfo.AdminLimited = elevationType == 3;
		}
	}

	if (tokenHandle)
	{
		CloseHandle(tokenHandle);
	}
	return userInfo;
}
DWORD KxSystem::GetLastError()
{
	return ::GetLastError();
}
wxString KxSystem::GetErrorMessage(DWORD messageID, DWORD langID)
{
	wxString string = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, messageID, langID);
	string.Truncate(string.Length()-1);
	return string;
}

wxString KxSystem::GetEnvironmentVariable(const wxString& name)
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
bool KxSystem::SetEnvironmentVariable(const wxString& name, const wxString& value)
{
	return ::SetEnvironmentVariableW(name.wc_str(), value.wc_str());
}
KxStringToStringUMap KxSystem::GetEnvironmentVariables()
{
	KxStringToStringUMap map;
	for (const wchar_t* item = ::GetEnvironmentStringsW(); *item; item += wcslen(item) + 1)
	{
		const wchar_t* separator = wcschr(item, L'=');
		map.insert_or_assign(wxString(item, separator - item), wxString(separator + 1));
	}
	return map;
}

bool KxSystem::IsWindowsServer()
{
	return GetVersionInfo().ProductType == VER_NT_SERVER;
}
bool KxSystem::IsWindowsVersionOrGreater(int majorVersion, int minorVersion, int servicePackMajor)
{
	VersionInfo version = GetVersionInfo();
	bool isVersionOK = version.Kernel.Major >= majorVersion && version.Kernel.Minor >= minorVersion;
	if (servicePackMajor != -1)
	{
		isVersionOK = isVersionOK && version.ServicePackMajor >= servicePackMajor;
	}
	return isVersionOK;
}

//////////////////////////////////////////////////////////////////////////
KxSystemFSRedirector::KxSystemFSRedirector()
{
	if (KxSystemAPI::Wow64DisableWow64FsRedirection)
	{
		KxSystemAPI::Wow64DisableWow64FsRedirection(&m_Value);
	}
}
KxSystemFSRedirector::~KxSystemFSRedirector()
{
	if (KxSystemAPI::Wow64RevertWow64FsRedirection && m_Value)
	{
		KxSystemAPI::Wow64RevertWow64FsRedirection(&m_Value);
		m_Value = nullptr;
	}
}
