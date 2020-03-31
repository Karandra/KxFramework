/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

#define KxSysAPI_DECLARE_LIBRARY(name)								static HMODULE Lib_##name
#define KxSysAPI_DEFINE_LIBRARY(name)								HMODULE KxSystemAPI::Lib_##name = nullptr
#define KxSysAPI_LOAD_LIBRARY(name)									Lib_##name = ::LoadLibraryW(L ## #name".dll")
#define KxSysAPI_UNLOAD_LIBRARY(name)								if (Lib_##name != nullptr) {::FreeLibrary(Lib_##name);}
#define KxSysAPI_GET_LIBRARY(name)									Lib_##name
#define KxSysAPI_CHECK_LIBRARY(name)								(KxSysAPI_GET_LIBRARY(name) != nullptr)

#define KxSysAPI_DECLARE_FUNCTION(ret_type, call_conv, name, ...)	\
	typedef ret_type (call_conv *FUNCTION_##name)(__VA_ARGS__);		\
	static FUNCTION_##name name;									\

#define KxSysAPI_DEFINE_FUNCTION(name)								KxSystemAPI::FUNCTION_##name KxSystemAPI::name = nullptr

#define KxSysAPI_INIT_FUNCTION_AS(dll, name, in_dll_name)			name = (FUNCTION_##name)GetProcAddress((Lib_##dll), #in_dll_name)
#define KxSysAPI_INIT_FUNCTION(dll, name)							KxSysAPI_INIT_FUNCTION_AS(dll, name, name)

class KX_API KxSystemAPI
{
	friend class KxSystemAPIModule;

	public:
		using MARGINS = void;
		using DWM_BLURBEHIND = void;
		using IMAGE_NT_HEADERS = void;
		using DPI_AWARENESS_CONTEXT = void*;

	private:
		static void InitFunctions();
		static void UnInitFunctions();

	public:
		KxSysAPI_DECLARE_LIBRARY(NTDLL);
		KxSysAPI_DECLARE_LIBRARY(Kernel32);
		KxSysAPI_DECLARE_LIBRARY(User32);
		KxSysAPI_DECLARE_LIBRARY(DWMAPI);
		KxSysAPI_DECLARE_LIBRARY(DbgHelp);

	public:
		/* NTDLL */
		KxSysAPI_DECLARE_FUNCTION(NTSTATUS, WINAPI, RtlGetVersion, RTL_OSVERSIONINFOEXW*);
		KxSysAPI_DECLARE_FUNCTION(NTSTATUS, NTAPI, NtQueryInformationProcess, HANDLE, int, PVOID, ULONG, PULONG);
		KxSysAPI_DECLARE_FUNCTION(ULONG, NTAPI, RtlNtStatusToDosError, ULONG);

		/* Kernel32 */
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, Wow64DisableWow64FsRedirection, PVOID*);
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, Wow64RevertWow64FsRedirection, PVOID);

		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, IsWow64Process, HANDLE, BOOL*);

		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, SetDefaultDllDirectories, DWORD);
		KxSysAPI_DECLARE_FUNCTION(DLL_DIRECTORY_COOKIE, WINAPI, AddDllDirectory, PCWSTR);
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, RemoveDllDirectory, DLL_DIRECTORY_COOKIE);
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, SetDllDirectoryW, LPCWSTR);
		KxSysAPI_DECLARE_FUNCTION(DWORD, WINAPI, GetDllDirectoryW, DWORD, LPWSTR);

		/* User32 */
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, EnableNonClientDpiScaling, HWND);
		KxSysAPI_DECLARE_FUNCTION(DPI_AWARENESS_CONTEXT, WINAPI, SetThreadDpiAwarenessContext, DPI_AWARENESS_CONTEXT);
		KxSysAPI_DECLARE_FUNCTION(UINT, WINAPI, GetDpiForSystem);
		KxSysAPI_DECLARE_FUNCTION(UINT, WINAPI, GetDpiForWindow, HWND);

		/* DWMAPI */
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmIsCompositionEnabled, BOOL*);
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmGetColorizationColor, DWORD*, BOOL*);
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmExtendFrameIntoClientArea, HWND, const MARGINS*);
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmEnableBlurBehindWindow, HWND, const DWM_BLURBEHIND*);

		/* DbgHelp */
		KxSysAPI_DECLARE_FUNCTION(IMAGE_NT_HEADERS*, WINAPI, ImageNtHeader, void*);
};
