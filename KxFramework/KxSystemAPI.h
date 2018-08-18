#pragma once
#include "KxFramework/KxFramework.h"

#define KxSysAPI_DECLARE_LIBRARY(name)								static HMODULE Lib_##name
#define KxSysAPI_DEFINE_LIBRARY(name)								HMODULE KxSystemAPI::Lib_##name = NULL
#define KxSysAPI_LOAD_LIBRARY(name)									Lib_##name = ::LoadLibraryW(L ## #name".dll")
#define KxSysAPI_UNLOAD_LIBRARY(name)								if (Lib_##name != NULL) {::FreeLibrary(Lib_##name);}
#define KxSysAPI_GET_LIBRARY(name)									Lib_##name
#define KxSysAPI_CHECK_LIBRARY(name)								(KxSysAPI_GET_LIBRARY(name) != NULL)

#define KxSysAPI_DECLARE_FUNCTION(ret_type, call_conv, name, ...)	\
	typedef ret_type (call_conv *FUNCTION_##name)(__VA_ARGS__);		\
	static FUNCTION_##name name;									\

#define KxSysAPI_DEFINE_FUNCTION(name)								KxSystemAPI::FUNCTION_##name KxSystemAPI::name = NULL

#define KxSysAPI_INIT_FUNCTION_AS(dll, name, in_dll_name)			name = (FUNCTION_##name)GetProcAddress((Lib_##dll), #in_dll_name)
#define KxSysAPI_INIT_FUNCTION(dll, name)							KxSysAPI_INIT_FUNCTION_AS(dll, name, name)

class KxSystemAPI
{
	friend class KxSystemAPIModule;

	public:
		typedef void MARGINS;
		typedef void DWM_BLURBEHIND;
		typedef void IMAGE_NT_HEADERS;

	private:
		static void InitFunctions();
		static void UnInitFunctions();

	public:
		KxSysAPI_DECLARE_LIBRARY(NTDLL);
		KxSysAPI_DECLARE_LIBRARY(Kernel32);
		KxSysAPI_DECLARE_LIBRARY(DWMAPI);
		KxSysAPI_DECLARE_LIBRARY(DbgHelp);

	public:
		/* NTDLL */
		KxSysAPI_DECLARE_FUNCTION(NTSTATUS, WINAPI, RtlGetVersion, RTL_OSVERSIONINFOEXW*);
		KxSysAPI_DECLARE_FUNCTION(NTSTATUS, NTAPI, NtQueryInformationProcess, HANDLE, int, PVOID, ULONG, PULONG);

		/* Kernel32 */
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, Wow64DisableWow64FsRedirection, PVOID*);
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, Wow64RevertWow64FsRedirection, PVOID);

		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, IsWow64Process, HANDLE, BOOL*);

		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, SetDefaultDllDirectories, DWORD);
		KxSysAPI_DECLARE_FUNCTION(DLL_DIRECTORY_COOKIE, WINAPI, AddDllDirectory, PCWSTR);
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, RemoveDllDirectory, DLL_DIRECTORY_COOKIE);
		KxSysAPI_DECLARE_FUNCTION(BOOL, WINAPI, SetDllDirectoryW, LPCWSTR);
		KxSysAPI_DECLARE_FUNCTION(DWORD, WINAPI, GetDllDirectoryW, DWORD, LPWSTR);

		/* DWMAPI */
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmIsCompositionEnabled, BOOL*);
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmGetColorizationColor, DWORD*, BOOL*);
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmExtendFrameIntoClientArea, HWND, const MARGINS*);
		KxSysAPI_DECLARE_FUNCTION(HRESULT, WINAPI, DwmEnableBlurBehindWindow, HWND, const DWM_BLURBEHIND*);

		/* DbgHelp */
		KxSysAPI_DECLARE_FUNCTION(IMAGE_NT_HEADERS*, WINAPI, ImageNtHeader, void*);
};
