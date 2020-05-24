#include "stdafx.h"
#include "NativeAPI.h"
#include "Kx/Utility/Common.h"
#include <Windows.h>
#include <wx/module.h>
#include <wx/log.h>
#include "UndefWindows.h"

#define DECLARE_LIBRARY(name)	\
static_assert(static_cast<size_t>(NativeLibrary::##name) < Utility::ArraySize<decltype(m_LoadedLibraries)>::value);	\
m_LoadedLibraries[static_cast<size_t>(NativeLibrary::##name)].Name = L#name".dll"

#define DEFINE_FUNCTION(name)										T##name name = nullptr
#define INIT_FUNCTION_AS(dll, name, in_dll_name)					dll##::name = reinterpret_cast<dll##::T##name>(::GetProcAddress(reinterpret_cast<HMODULE>(m_LoadedLibraries[static_cast<size_t>(NativeLibrary::##dll)].Handle), #in_dll_name))
#define INIT_FUNCTION(dll, name)									INIT_FUNCTION_AS(dll, name, name)

namespace kxf::NativeAPI::Private
{
	Loader::Loader() noexcept
	{
		DECLARE_LIBRARY(NtDLL);
		DECLARE_LIBRARY(Kernel32);
		DECLARE_LIBRARY(User32);
		DECLARE_LIBRARY(DWMAPI);
		DECLARE_LIBRARY(DbgHelp);
	}

	size_t Loader::LoadLibraries() noexcept
	{
		size_t count = 0;
		for (LibraryRecord& library: m_LoadedLibraries)
		{
			library.Handle = ::LoadLibraryW(library.Name);
			if (library.Handle)
			{
				count++;
			}
			else
			{
				wxLogWarning("Couldn't loaded \"%s\" library", library.Name);
			}
		}
		return count;
	}
	size_t Loader::UnloadLibraries() noexcept
	{
		size_t count = 0;
		for (LibraryRecord& library: m_LoadedLibraries)
		{
			if (library.Handle)
			{
				::FreeLibrary(reinterpret_cast<HMODULE>(library.Handle));
				library.Handle = nullptr;
				count++;
			}
		}
		return count;
	}
	bool Loader::IsLibraryLoaded(NativeLibrary library) const noexcept
	{
		const size_t index = static_cast<size_t>(library);
		return index < m_LoadedLibraries.size() && m_LoadedLibraries[index].Handle;
	}

	void Loader::LoadNtDLL() noexcept
	{
		if (IsLibraryLoaded(NativeLibrary::NtDLL))
		{
			INIT_FUNCTION(NtDLL, RtlGetVersion);
			INIT_FUNCTION(NtDLL, NtQueryInformationProcess);
			INIT_FUNCTION(NtDLL, RtlNtStatusToDosError);
			INIT_FUNCTION(NtDLL, NtSuspendProcess);
			INIT_FUNCTION(NtDLL, NtResumeProcess);
		}
	}
	void Loader::LoadKernel32() noexcept
	{
		if (IsLibraryLoaded(NativeLibrary::Kernel32))
		{
			INIT_FUNCTION(Kernel32, Wow64DisableWow64FsRedirection);
			INIT_FUNCTION(Kernel32, Wow64RevertWow64FsRedirection);

			INIT_FUNCTION(Kernel32, IsWow64Process);

			INIT_FUNCTION(Kernel32, SetDefaultDllDirectories);
			INIT_FUNCTION(Kernel32, AddDllDirectory);
			INIT_FUNCTION(Kernel32, RemoveDllDirectory);
			INIT_FUNCTION(Kernel32, SetDllDirectoryW);
			INIT_FUNCTION(Kernel32, GetDllDirectoryW);
		}
	}
	void Loader::LoadUser32() noexcept
	{
		if (IsLibraryLoaded(NativeLibrary::User32))
		{
			INIT_FUNCTION(User32, EnableNonClientDpiScaling);
			INIT_FUNCTION(User32, SetThreadDpiAwarenessContext);
			INIT_FUNCTION(User32, GetDpiForSystem);
			INIT_FUNCTION(User32, GetDpiForWindow);
		}
	}
	void Loader::LoadDWMAPI() noexcept
	{
		if (IsLibraryLoaded(NativeLibrary::DWMAPI))
		{
			INIT_FUNCTION(DWMAPI, DwmIsCompositionEnabled);
			INIT_FUNCTION(DWMAPI, DwmGetColorizationColor);
			INIT_FUNCTION(DWMAPI, DwmExtendFrameIntoClientArea);
			INIT_FUNCTION(DWMAPI, DwmEnableBlurBehindWindow);
		}
	}
	void Loader::LoadDbgHelp() noexcept
	{
		if (IsLibraryLoaded(NativeLibrary::DbgHelp))
		{
			INIT_FUNCTION(DbgHelp, ImageNtHeader);
		}
	}
}

namespace kxf::NativeAPI::Private
{
	class InitializationModule final: public wxModule
	{
		private:
			Loader m_Loader;

		public:
			bool OnInit() noexcept override
			{
				if (m_Loader.LoadLibraries() != 0)
				{
					m_Loader.LoadNtDLL();
					m_Loader.LoadKernel32();
					m_Loader.LoadUser32();
					m_Loader.LoadDWMAPI();
					m_Loader.LoadDbgHelp();

					return true;
				}
				return false;
			}
			void OnExit() noexcept override
			{
				m_Loader.UnloadLibraries();
			}

		private:
			wxDECLARE_DYNAMIC_CLASS(InitializationModule);
	};
	wxIMPLEMENT_DYNAMIC_CLASS(InitializationModule, wxModule);
}

namespace kxf::NativeAPI
{
	namespace NtDLL
	{
		DEFINE_FUNCTION(RtlGetVersion);
		DEFINE_FUNCTION(NtQueryInformationProcess);
		DEFINE_FUNCTION(RtlNtStatusToDosError);
		DEFINE_FUNCTION(NtSuspendProcess);
		DEFINE_FUNCTION(NtResumeProcess);
	}
	namespace Kernel32
	{
		DEFINE_FUNCTION(Wow64DisableWow64FsRedirection);
		DEFINE_FUNCTION(Wow64RevertWow64FsRedirection);

		DEFINE_FUNCTION(IsWow64Process);

		DEFINE_FUNCTION(SetDefaultDllDirectories);
		DEFINE_FUNCTION(AddDllDirectory);
		DEFINE_FUNCTION(RemoveDllDirectory);
		DEFINE_FUNCTION(SetDllDirectoryW);
		DEFINE_FUNCTION(GetDllDirectoryW);
	}
	namespace User32
	{
		DEFINE_FUNCTION(EnableNonClientDpiScaling);
		DEFINE_FUNCTION(SetThreadDpiAwarenessContext);
		DEFINE_FUNCTION(GetDpiForSystem);
		DEFINE_FUNCTION(GetDpiForWindow);
	}
	namespace DWMAPI
	{
		DEFINE_FUNCTION(DwmIsCompositionEnabled);
		DEFINE_FUNCTION(DwmGetColorizationColor);
		DEFINE_FUNCTION(DwmExtendFrameIntoClientArea);
		DEFINE_FUNCTION(DwmEnableBlurBehindWindow);
	}
	namespace DbgHelp
	{
		DEFINE_FUNCTION(ImageNtHeader);
	}
}
