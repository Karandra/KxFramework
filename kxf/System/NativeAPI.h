#pragma once
#include "Common.h"
#include "UndefWindows.h"
#include <array>

namespace kxf
{
	class FSPath;

	enum class NativeAPISet
	{
		None = -1,

		NtDLL,
		Kernel32,
		KernelBase,
		User32,
		ShlWAPI,
		DWMAPI,
		DbgHelp,
		DXGI,
		DComp
	};
}
namespace kxf::NativeAPI::Private
{
	class InitializationModule;
}

namespace kxf
{
	class KX_API NativeAPILoader final
	{
		friend class NativeAPI::Private::InitializationModule;

		private:
			struct LibraryRecord final
			{
				public:
					void* Handle = nullptr;
					const wchar_t* Name = nullptr;
					NativeAPISet Type = NativeAPISet::None;

				public:
					bool IsNull() const noexcept
					{
						return Handle == nullptr || Name == nullptr || Type == NativeAPISet::None;
					}
			};

		public:
			static NativeAPILoader& GetInstance();

		private:
			std::array<LibraryRecord, 16> m_LoadedLibraries = {};
			String m_LookupDirectory;
			size_t m_TotalCount = 0;

		private:
			size_t GetLoadedLibrariesCount() const noexcept;
			size_t DoLoadLibraries(std::initializer_list<NativeAPISet> apiSets, bool moduleMode = false) noexcept;
			size_t DoUnloadLibraries() noexcept;

			void InitializeNtDLL() noexcept;
			void InitializeKernel32() noexcept;
			void InitializeKernelBase() noexcept;
			void InitializeUser32() noexcept;
			void InitializeShlWAPI() noexcept;
			void InitializeDWMAPI() noexcept;
			void InitializeDbgHelp() noexcept;
			void InitializeDXGI() noexcept;
			void InitializeDComp() noexcept;

		private:
			NativeAPILoader() noexcept;

		public:
			size_t LoadLibraries() noexcept
			{
				return DoLoadLibraries({});
			}
			size_t LoadLibraries(std::initializer_list<NativeAPISet> apiSets) noexcept
			{
				return apiSets.size() != 0 ? DoLoadLibraries(std::move(apiSets)) : 0;
			}
			size_t UnloadLibraries() noexcept
			{
				return DoUnloadLibraries();
			}
			void Initialize();
			bool IsLibraryLoaded(NativeAPISet library) const noexcept;

			void SetLookupDirectory(const FSPath& path);
	};
}

#define Kx_NativeAPI	__stdcall

#define Kx_NativeAPI_DeclateFunc(ret_type, call_conv, name, ...)	\
	using T##name = ret_type (call_conv*)(__VA_ARGS__);	\
	extern T##name name

struct _GUID;
struct IDXGIDevice;

namespace kxf::NativeAPI
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
		using NTSTATUS = int32_t;
		using HRESULT = int32_t;
		using GUID = ::_GUID;
		using IID = ::_GUID;

		using LDR_DLL_NOTIFICATION_DATA = void;
		using LDR_DLL_NOTIFICATION_FUNCTION = void(Kx_NativeAPI*)(ULONG reason, const LDR_DLL_NOTIFICATION_DATA* data, void* context);
	}

	namespace NtDLL
	{
		Kx_NativeAPI_DeclateFunc(NTSTATUS, Kx_NativeAPI, RtlGetVersion, RTL_OSVERSIONINFOEXW*);
		Kx_NativeAPI_DeclateFunc(NTSTATUS, Kx_NativeAPI, NtQueryInformationProcess, HANDLE, int, void*, ULONG, ULONG*);
		Kx_NativeAPI_DeclateFunc(NTSTATUS, Kx_NativeAPI, RtlGetLastNtStatus);
		Kx_NativeAPI_DeclateFunc(void, Kx_NativeAPI, RtlSetLastWin32ErrorAndNtStatusFromNtStatus, NTSTATUS);
		Kx_NativeAPI_DeclateFunc(ULONG, Kx_NativeAPI, RtlNtStatusToDosError, ULONG);
		Kx_NativeAPI_DeclateFunc(LONG, Kx_NativeAPI, NtSuspendProcess, HANDLE);
		Kx_NativeAPI_DeclateFunc(LONG, Kx_NativeAPI, NtResumeProcess, HANDLE);
		Kx_NativeAPI_DeclateFunc(NTSTATUS, Kx_NativeAPI, LdrRegisterDllNotification, ULONG, LDR_DLL_NOTIFICATION_FUNCTION, void*, void**);
		Kx_NativeAPI_DeclateFunc(NTSTATUS, Kx_NativeAPI, LdrUnregisterDllNotification, void*);
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
	namespace KernelBase
	{
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, PathCchCanonicalizeEx, wchar_t*, size_t, const wchar_t*, ULONG);
	}
	namespace User32
	{
		Kx_NativeAPI_DeclateFunc(UINT, Kx_NativeAPI, GetDpiForSystem);
		Kx_NativeAPI_DeclateFunc(UINT, Kx_NativeAPI, GetDpiForWindow, HWND);
		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, EnableNonClientDpiScaling, HWND);
		Kx_NativeAPI_DeclateFunc(DPI_AWARENESS_CONTEXT, Kx_NativeAPI, SetThreadDpiAwarenessContext, DPI_AWARENESS_CONTEXT);
	}
	namespace ShlWAPI
	{
		Kx_NativeAPI_DeclateFunc(BOOL, Kx_NativeAPI, PathCanonicalizeW, wchar_t*, const wchar_t*);
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
	namespace DXGI
	{
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, CreateDXGIFactory1, const ::IID&, void**);
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, CreateDXGIFactory2, UINT, const ::IID&, void**);
	}
	namespace DComp
	{
		Kx_NativeAPI_DeclateFunc(HRESULT, Kx_NativeAPI, DCompositionCreateDevice, IDXGIDevice*, const ::IID&, void**);
	}
}

#undef Kx_NativeAPI
#undef Kx_NativeAPI_DeclateFunc
