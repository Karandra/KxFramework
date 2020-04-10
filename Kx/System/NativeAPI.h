#pragma once
#include "Common.h"
#include "ErrorCodeValue.h"
#include "UndefWindows.h"

namespace KxFramework::NativeAPI
{
	enum class NativeLibrary
	{
		NtDLL,
		Kernel32,
		User32,
		DWMAPI,
		DbgHelp,
	};
}

namespace KxFramework::NativeAPI::Private
{
	using MARGINS = void;
	using DWM_BLURBEHIND = void;
	using IMAGE_NT_HEADERS = void;
	using DPI_AWARENESS_CONTEXT = void*;
	using RTL_OSVERSIONINFOEXW = void;
	using HANDLE = void*;
	using NTSTATUS = NtStatusCode::TValueType;

	class Loader final
	{
		friend class InitializationModule;
		struct LibraryRecord final
		{
			HMODULE Handle = nullptr;
			const wchar_t* Name = nullptr;
		};

		private:
			std::array<LibraryRecord, 5> m_LoadedLibraries = {};

		private:
			Loader() noexcept;

		public:
			size_t LoadLibraries() noexcept;
			size_t UnloadLibraries() noexcept;
			bool IsLibraryLoaded(NativeLibrary library) const noexcept;

			void LoadNtDLL() noexcept;
			void LoadKernel32() noexcept;
			void LoadUser32() noexcept;
			void LoadDWMAPI() noexcept;
			void LoadDbgHelp() noexcept;
	};
}

#define Kx_NativeAPI_DeclateFunc(ret_type, call_conv, name, ...)	\
	using T##name = ret_type (call_conv*)(__VA_ARGS__);	\
	extern T##name name

namespace KxFramework::NativeAPI
{
	namespace
	{
		using MARGINS = void;
		using DWM_BLURBEHIND = void;
		using IMAGE_NT_HEADERS = void;
		using DPI_AWARENESS_CONTEXT = void*;
		using RTL_OSVERSIONINFOEXW = void;
		using DLL_DIRECTORY_COOKIE = void*;
		using HANDLE = void*;
		using NTSTATUS = NtStatusCode::TValueType;
		using HRESULT = HResultCode::TValueType;
	}

	namespace NtDLL
	{
		Kx_NativeAPI_DeclateFunc(NTSTATUS, WINAPI, RtlGetVersion, RTL_OSVERSIONINFOEXW*);
		Kx_NativeAPI_DeclateFunc(NTSTATUS, NTAPI, NtQueryInformationProcess, HANDLE, int, void*, ULONG, ULONG*);
		Kx_NativeAPI_DeclateFunc(ULONG, NTAPI, RtlNtStatusToDosError, ULONG);
	}
	namespace Kernel32
	{
		Kx_NativeAPI_DeclateFunc(BOOL, WINAPI, Wow64DisableWow64FsRedirection, void**);
		Kx_NativeAPI_DeclateFunc(BOOL, WINAPI, Wow64RevertWow64FsRedirection, void*);

		Kx_NativeAPI_DeclateFunc(BOOL, WINAPI, IsWow64Process, HANDLE, BOOL*);

		Kx_NativeAPI_DeclateFunc(BOOL, WINAPI, SetDefaultDllDirectories, DWORD);
		Kx_NativeAPI_DeclateFunc(DLL_DIRECTORY_COOKIE, WINAPI, AddDllDirectory, PCWSTR);
		Kx_NativeAPI_DeclateFunc(BOOL, WINAPI, RemoveDllDirectory, DLL_DIRECTORY_COOKIE);
		Kx_NativeAPI_DeclateFunc(BOOL, WINAPI, SetDllDirectoryW, LPCWSTR);
		Kx_NativeAPI_DeclateFunc(DWORD, WINAPI, GetDllDirectoryW, DWORD, LPWSTR);
	}
	namespace User32
	{
		Kx_NativeAPI_DeclateFunc(UINT, WINAPI, GetDpiForSystem);
		Kx_NativeAPI_DeclateFunc(UINT, WINAPI, GetDpiForWindow, HWND);
		Kx_NativeAPI_DeclateFunc(BOOL, WINAPI, EnableNonClientDpiScaling, HWND);
		Kx_NativeAPI_DeclateFunc(DPI_AWARENESS_CONTEXT, WINAPI, SetThreadDpiAwarenessContext, DPI_AWARENESS_CONTEXT);
	}
	namespace DWMAPI
	{
		Kx_NativeAPI_DeclateFunc(HRESULT, WINAPI, DwmIsCompositionEnabled, BOOL*);
		Kx_NativeAPI_DeclateFunc(HRESULT, WINAPI, DwmGetColorizationColor, DWORD*, BOOL*);
		Kx_NativeAPI_DeclateFunc(HRESULT, WINAPI, DwmExtendFrameIntoClientArea, HWND, const MARGINS*);
		Kx_NativeAPI_DeclateFunc(HRESULT, WINAPI, DwmEnableBlurBehindWindow, HWND, const DWM_BLURBEHIND*);
	}
	namespace DbgHelp
	{
		Kx_NativeAPI_DeclateFunc(IMAGE_NT_HEADERS*, WINAPI, ImageNtHeader, void*);
	}
}

#undef Kx_NativeAPI_DeclateFunc
