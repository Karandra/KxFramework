#pragma once
#include "Common.h"
#include "ErrorCodeValue.h"
#include "UndefWindows.h"
#include <array>

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
	class Loader final
	{
		friend class InitializationModule;
		struct LibraryRecord final
		{
			void* Handle = nullptr;
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

#define Kx_NativeAPI	__stdcall

#define Kx_NativeAPI_DeclateFunc(ret_type, call_conv, name, ...)	\
	using T##name = ret_type (call_conv*)(__VA_ARGS__);	\
	extern T##name name

namespace KxFramework::NativeAPI
{
	namespace
	{
		using BOOL = int32_t;
		using LONG = int32_t;
		using UINT = uint32_t;
		using ULONG = uint32_t;
		using DWORD = uint32_t;

		using HWND = void*;
		using HANDLE = void*;
		using MARGINS = void;
		using DWM_BLURBEHIND = void;
		using IMAGE_NT_HEADERS = void;
		using DPI_AWARENESS_CONTEXT = void*;
		using RTL_OSVERSIONINFOEXW = void;
		using DLL_DIRECTORY_COOKIE = void*;
		using NTSTATUS = NtStatus::TValueType;
		using HRESULT = HResult::TValueType;
	}

	namespace NtDLL
	{
		Kx_NativeAPI_DeclateFunc(NTSTATUS, Kx_NativeAPI, RtlGetVersion, RTL_OSVERSIONINFOEXW*);
		Kx_NativeAPI_DeclateFunc(NTSTATUS, Kx_NativeAPI, NtQueryInformationProcess, HANDLE, int, void*, ULONG, ULONG*);
		Kx_NativeAPI_DeclateFunc(ULONG, Kx_NativeAPI, RtlNtStatusToDosError, ULONG);
		Kx_NativeAPI_DeclateFunc(LONG, Kx_NativeAPI, NtSuspendProcess, HANDLE);
		Kx_NativeAPI_DeclateFunc(LONG, Kx_NativeAPI, NtResumeProcess, HANDLE);
	}
	namespace Kernel32
	{
		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, Wow64DisableWow64FsRedirection, void**);
		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, Wow64RevertWow64FsRedirection, void*);

		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, IsWow64Process, HANDLE, BOOL*);

		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, SetDefaultDllDirectories, DWORD);
		Kx_NativeAPI_DeclateFunc(DLL_DIRECTORY_COOKIE, Kx_NativeAPI, AddDllDirectory, const wchar_t*);
		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, RemoveDllDirectory, DLL_DIRECTORY_COOKIE);
		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, SetDllDirectoryW, const wchar_t*);
		Kx_NativeAPI_DeclateFunc(DWORD, Kx_NativeAPI, GetDllDirectoryW, DWORD, wchar_t*);
	}
	namespace User32
	{
		Kx_NativeAPI_DeclateFunc(UINT, Kx_NativeAPI, GetDpiForSystem);
		Kx_NativeAPI_DeclateFunc(UINT, Kx_NativeAPI, GetDpiForWindow, HWND);
		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, EnableNonClientDpiScaling, HWND);
		Kx_NativeAPI_DeclateFunc(DPI_AWARENESS_CONTEXT, Kx_NativeAPI, SetThreadDpiAwarenessContext, DPI_AWARENESS_CONTEXT);
	}
	namespace DWMAPI
	{
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, DwmIsCompositionEnabled, BOOL*);
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, DwmGetColorizationColor, DWORD*, BOOL*);
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, DwmExtendFrameIntoClientArea, HWND, const MARGINS*);
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, DwmEnableBlurBehindWindow, HWND, const DWM_BLURBEHIND*);
	}
	namespace DbgHelp
	{
		Kx_NativeAPI_DeclateFunc(IMAGE_NT_HEADERS*, Kx_NativeAPI, ImageNtHeader, void*);
	}
}

#undef Kx_NativeAPI
#undef Kx_NativeAPI_DeclateFunc
