#include "KxfPCH.h"
#include "NativeAPI.h"
#include "kxf/Log/Common.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Utility/Common.h"
#include <Windows.h>
#include <wx/module.h>
#include "UndefWindows.h"

#define DECLARE_LIBRARY(name)	\
static_assert(static_cast<size_t>(NativeAPISet::##name) < Utility::ArraySize<decltype(m_LoadedLibraries)>::value);	\
{	\
	auto& item = m_LoadedLibraries[static_cast<size_t>(NativeAPISet::##name)];	\
	\
	item.Name = L#name L".dll";	\
	item.Type = NativeAPISet::name;	\
	\
	m_TotalCount++;	\
}

#define DEFINE_FUNCTION(name)										T##name name = nullptr
#define INIT_FUNCTION_AS(dll, name, in_dll_name)					dll##::name = reinterpret_cast<dll##::T##name>(::GetProcAddress(reinterpret_cast<HMODULE>(m_LoadedLibraries[static_cast<size_t>(NativeAPISet::##dll)].Handle), #in_dll_name))
#define INIT_FUNCTION(dll, name)									INIT_FUNCTION_AS(dll, name, name)

namespace kxf
{
	NativeAPILoader& NativeAPILoader::GetInstance()
	{
		static NativeAPILoader instance;
		return instance;
	}

	NativeAPILoader::NativeAPILoader() noexcept
	{
		m_LoadedLibraries.fill({});

		using namespace NativeAPI;
		DECLARE_LIBRARY(NtDLL);
		DECLARE_LIBRARY(Kernel32);
		DECLARE_LIBRARY(KernelBase);
		DECLARE_LIBRARY(User32);
		DECLARE_LIBRARY(ShlWAPI);
		DECLARE_LIBRARY(DWMAPI);
		DECLARE_LIBRARY(DbgHelp);
		DECLARE_LIBRARY(DXGI);
		DECLARE_LIBRARY(DComp);
	}

	size_t NativeAPILoader::GetLoadedLibrariesCount() const noexcept
	{
		return std::ranges::count_if(m_LoadedLibraries, [](const auto& item)
		{
			return item.Handle != nullptr;
		});
	}
	size_t NativeAPILoader::DoLoadLibraries(std::initializer_list<NativeAPISet> apiSets, bool moduleMode) noexcept
	{
		const size_t loadedAlready = GetLoadedLibrariesCount();
		Log::Info("DoLoadLibraries: loadedAlready=[{}], m_TotalCount=[{}], moduleMode=[{}]", loadedAlready, m_TotalCount, moduleMode);
		Log::Info("Lookup directory: '{}'", m_LookupDirectory);

		if (moduleMode && loadedAlready != 0)
		{
			return loadedAlready;
		}

		size_t loadedCount = 0;
		for (size_t i = 0; i < m_TotalCount; i++)
		{
			auto& item = m_LoadedLibraries[i];
			if (apiSets.size() == 0 || std::find(apiSets.begin(), apiSets.end(), item.Type) != apiSets.end())
			{
				Log::Info("Loading library: '{}'", item.Name);

				if (!m_LookupDirectory.IsEmpty())
				{
					FSPath path = m_LookupDirectory;
					path.Append(item.Name);

					auto fullPath = path.GetFullPathWithNS();
					Log::Info("Loading library '{}' using a fully qualified path name '{}'", item.Name, fullPath);

					item.Handle = ::LoadLibraryW(fullPath.wc_str());
				}
				else
				{
					item.Handle = ::LoadLibraryW(item.Name);
				}

				if (item.Handle)
				{
					loadedCount++;
				}
				else
				{
					Log::Warning("Couldn't load '{}' library", item.Name);
				}
			}
		}

		Log::Info("Loaded {} libraries", loadedCount);
		return GetLoadedLibrariesCount();
	}
	size_t NativeAPILoader::DoUnloadLibraries() noexcept
	{
		size_t unloadedCount = 0;
		for (size_t i = 0; i < m_TotalCount; i++)
		{
			if (auto& item = m_LoadedLibraries[i]; item.Handle)
			{
				::FreeLibrary(reinterpret_cast<HMODULE>(item.Handle));
				item.Handle = nullptr;

				unloadedCount++;
			}
		}
		return unloadedCount;
	}

	void NativeAPILoader::InitializeNtDLL() noexcept
	{
		Log::Info("InitializeNtDLL");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::NtDLL))
		{
			INIT_FUNCTION(NtDLL, RtlGetVersion);
			INIT_FUNCTION(NtDLL, NtQueryInformationProcess);
			INIT_FUNCTION(NtDLL, RtlGetLastNtStatus);
			INIT_FUNCTION(NtDLL, RtlSetLastWin32ErrorAndNtStatusFromNtStatus);
			INIT_FUNCTION(NtDLL, RtlNtStatusToDosError);
			INIT_FUNCTION(NtDLL, NtSuspendProcess);
			INIT_FUNCTION(NtDLL, NtResumeProcess);
			INIT_FUNCTION(NtDLL, LdrRegisterDllNotification);
			INIT_FUNCTION(NtDLL, LdrUnregisterDllNotification);

			Log::Info("InitializeNtDLL -> Success");
		}
	}
	void NativeAPILoader::InitializeKernel32() noexcept
	{
		Log::Info("InitializeKernel32");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::Kernel32))
		{
			INIT_FUNCTION(Kernel32, Wow64DisableWow64FsRedirection);
			INIT_FUNCTION(Kernel32, Wow64RevertWow64FsRedirection);

			INIT_FUNCTION(Kernel32, IsWow64Process);
			INIT_FUNCTION(Kernel32, IsWow64Process2);

			INIT_FUNCTION(Kernel32, SetDefaultDllDirectories);
			INIT_FUNCTION(Kernel32, AddDllDirectory);
			INIT_FUNCTION(Kernel32, RemoveDllDirectory);
			INIT_FUNCTION(Kernel32, SetDllDirectoryW);
			INIT_FUNCTION(Kernel32, GetDllDirectoryW);
			INIT_FUNCTION(Kernel32, GetThreadDescription);
			INIT_FUNCTION(Kernel32, SetThreadDescription);

			Log::Info("InitializeKernel32 -> Success");
		}
	}
	void NativeAPILoader::InitializeKernelBase() noexcept
	{
		Log::Info("InitializeKernelBase");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::KernelBase))
		{
			INIT_FUNCTION(KernelBase, PathCchCanonicalizeEx);

			Log::Info("InitializeKernelBase -> Success");
		}
	}
	void NativeAPILoader::InitializeUser32() noexcept
	{
		Log::Info("InitializeUser32");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::User32))
		{
			INIT_FUNCTION(User32, EnableNonClientDpiScaling);
			INIT_FUNCTION(User32, SetThreadDpiAwarenessContext);
			INIT_FUNCTION(User32, GetDpiForSystem);
			INIT_FUNCTION(User32, GetDpiForWindow);

			Log::Info("InitializeUser32 -> Success");
		}
	}
	void NativeAPILoader::InitializeShlWAPI() noexcept
	{
		Log::Info("InitializeShlWAPI");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::ShlWAPI))
		{
			INIT_FUNCTION(ShlWAPI, PathCanonicalizeW);

			Log::Info("InitializeShlWAPI -> Success");
		}
	}
	void NativeAPILoader::InitializeDWMAPI() noexcept
	{
		Log::Info("InitializeDWMAPI");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DWMAPI))
		{
			INIT_FUNCTION(DWMAPI, DwmIsCompositionEnabled);
			INIT_FUNCTION(DWMAPI, DwmGetColorizationColor);
			INIT_FUNCTION(DWMAPI, DwmExtendFrameIntoClientArea);
			INIT_FUNCTION(DWMAPI, DwmEnableBlurBehindWindow);

			Log::Info("InitializeDWMAPI -> Success");
		}
	}
	void NativeAPILoader::InitializeDbgHelp() noexcept
	{
		Log::Info("InitializeDbgHelp");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DbgHelp))
		{
			INIT_FUNCTION(DbgHelp, ImageNtHeader);

			Log::Info("InitializeDbgHelp -> Success");
		}
	}
	void NativeAPILoader::InitializeDXGI() noexcept
	{
		Log::Info("InitializeDXGI");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DXGI))
		{
			INIT_FUNCTION(DXGI, CreateDXGIFactory1);
			INIT_FUNCTION(DXGI, CreateDXGIFactory2);

			Log::Info("InitializeDXGI -> Success");
		}
	}
	void NativeAPILoader::InitializeDComp() noexcept
	{
		Log::Info("InitializeDComp");
		using namespace NativeAPI;

		if (IsLibraryLoaded(NativeAPISet::DComp))
		{
			INIT_FUNCTION(DComp, DCompositionCreateDevice);

			Log::Info("InitializeDComp -> Success");
		}
	}

	void NativeAPILoader::Initialize()
	{
		InitializeNtDLL();
		InitializeKernel32();
		InitializeKernelBase();
		InitializeUser32();
		InitializeShlWAPI();
		InitializeDWMAPI();
		InitializeDbgHelp();
		InitializeDXGI();
		InitializeDComp();
	}
	bool NativeAPILoader::IsLibraryLoaded(NativeAPISet library) const noexcept
	{
		const size_t index = static_cast<size_t>(library);
		return index < m_LoadedLibraries.size() && m_LoadedLibraries[index].Handle;
	}

	void NativeAPILoader::SetLookupDirectory(const FSPath& path)
	{
		m_LookupDirectory = path.GetFullPathWithNS();
	}
}

namespace kxf::NativeAPI
{
	namespace NtDLL
	{
		DEFINE_FUNCTION(RtlGetVersion);
		DEFINE_FUNCTION(NtQueryInformationProcess);
		DEFINE_FUNCTION(RtlGetLastNtStatus);
		DEFINE_FUNCTION(RtlSetLastWin32ErrorAndNtStatusFromNtStatus);
		DEFINE_FUNCTION(RtlNtStatusToDosError);
		DEFINE_FUNCTION(NtSuspendProcess);
		DEFINE_FUNCTION(NtResumeProcess);
		DEFINE_FUNCTION(LdrRegisterDllNotification);
		DEFINE_FUNCTION(LdrUnregisterDllNotification);
	}
	namespace Kernel32
	{
		DEFINE_FUNCTION(Wow64DisableWow64FsRedirection);
		DEFINE_FUNCTION(Wow64RevertWow64FsRedirection);

		DEFINE_FUNCTION(IsWow64Process);
		DEFINE_FUNCTION(IsWow64Process2);

		DEFINE_FUNCTION(SetDefaultDllDirectories);
		DEFINE_FUNCTION(AddDllDirectory);
		DEFINE_FUNCTION(RemoveDllDirectory);
		DEFINE_FUNCTION(SetDllDirectoryW);
		DEFINE_FUNCTION(GetDllDirectoryW);
		DEFINE_FUNCTION(GetThreadDescription);
		DEFINE_FUNCTION(SetThreadDescription);
	}
	namespace KernelBase
	{
		DEFINE_FUNCTION(PathCchCanonicalizeEx);
	}
	namespace User32
	{
		DEFINE_FUNCTION(EnableNonClientDpiScaling);
		DEFINE_FUNCTION(SetThreadDpiAwarenessContext);
		DEFINE_FUNCTION(GetDpiForSystem);
		DEFINE_FUNCTION(GetDpiForWindow);
	}
	namespace ShlWAPI
	{
		DEFINE_FUNCTION(PathCanonicalizeW);
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
	namespace DXGI
	{
		DEFINE_FUNCTION(CreateDXGIFactory1);
		DEFINE_FUNCTION(CreateDXGIFactory2);
	}
	namespace DComp
	{
		DEFINE_FUNCTION(DCompositionCreateDevice);
	}
}

namespace kxf::NativeAPI::Private
{
	class InitializationModule final: public wxModule
	{
		private:
			NativeAPILoader& m_Loader = NativeAPILoader::GetInstance();

		public:
			bool OnInit() noexcept override
			{
				Log::Info("InitializationModule::OnInit");

				// In module mode the loader will load all libraries only if there are no libraries loaded yet.
				// We're still going to initialize them all, the initialization function checks the load state.
				if (m_Loader.DoLoadLibraries({}, true) != 0)
				{
					m_Loader.Initialize();

					Log::Info("InitializationModule::OnInit -> Success");
					return true;
				}
				else
				{
					Log::Info("InitializationModule::OnInit -> Failed");
					return false;
				}
			}
			void OnExit() noexcept override
			{
				Log::Info("InitializationModule::OnExit");

				m_Loader.UnloadLibraries();
			}

		private:
			wxDECLARE_DYNAMIC_CLASS(InitializationModule);
	};
}

wxIMPLEMENT_DYNAMIC_CLASS(kxf::NativeAPI::Private::InitializationModule, wxModule);
