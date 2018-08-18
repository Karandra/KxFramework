#include "KxStdAfx.h"
#include "KxFramework/KxSystemAPI.h"

/* Libraries */
KxSysAPI_DEFINE_LIBRARY(NTDLL);
KxSysAPI_DEFINE_LIBRARY(Kernel32);
KxSysAPI_DEFINE_LIBRARY(DWMAPI);
KxSysAPI_DEFINE_LIBRARY(DbgHelp);

/* NTDLL */
KxSysAPI_DEFINE_FUNCTION(RtlGetVersion);
KxSysAPI_DEFINE_FUNCTION(NtQueryInformationProcess);

/* Kernel32 */
KxSysAPI_DEFINE_FUNCTION(Wow64DisableWow64FsRedirection);
KxSysAPI_DEFINE_FUNCTION(Wow64RevertWow64FsRedirection);

KxSysAPI_DEFINE_FUNCTION(IsWow64Process);

KxSysAPI_DEFINE_FUNCTION(SetDefaultDllDirectories);
KxSysAPI_DEFINE_FUNCTION(AddDllDirectory);
KxSysAPI_DEFINE_FUNCTION(RemoveDllDirectory);
KxSysAPI_DEFINE_FUNCTION(SetDllDirectoryW);
KxSysAPI_DEFINE_FUNCTION(GetDllDirectoryW);

/* DWMAPI */
KxSysAPI_DEFINE_FUNCTION(DwmIsCompositionEnabled);
KxSysAPI_DEFINE_FUNCTION(DwmGetColorizationColor);
KxSysAPI_DEFINE_FUNCTION(DwmExtendFrameIntoClientArea);
KxSysAPI_DEFINE_FUNCTION(DwmEnableBlurBehindWindow);

/* DbgHelp */
KxSysAPI_DEFINE_FUNCTION(ImageNtHeader);

//////////////////////////////////////////////////////////////////////////
void KxSystemAPI::InitFunctions()
{
	KxSysAPI_LOAD_LIBRARY(NTDLL);
	KxSysAPI_LOAD_LIBRARY(Kernel32);
	KxSysAPI_LOAD_LIBRARY(DWMAPI);
	KxSysAPI_LOAD_LIBRARY(DbgHelp);

	if (KxSysAPI_CHECK_LIBRARY(NTDLL))
	{
		KxSysAPI_INIT_FUNCTION(NTDLL, RtlGetVersion);
		KxSysAPI_INIT_FUNCTION(NTDLL, NtQueryInformationProcess);
	}
	else
	{
		wxLogWarning("NTDLL is unavailable");
	}

	if (KxSysAPI_CHECK_LIBRARY(Kernel32))
	{
		if (!KxSysAPI_CHECK_LIBRARY(NTDLL))
		{
			KxSysAPI_INIT_FUNCTION_AS(Kernel32, RtlGetVersion, GetVersionExW);
			wxLogWarning("NTDLL::RtlGetVersion is unavailable. Using Kernel32::GetVersionExW instead");
		}

		KxSysAPI_INIT_FUNCTION(Kernel32, Wow64DisableWow64FsRedirection);
		KxSysAPI_INIT_FUNCTION(Kernel32, Wow64RevertWow64FsRedirection);

		KxSysAPI_INIT_FUNCTION(Kernel32, IsWow64Process);

		KxSysAPI_INIT_FUNCTION(Kernel32, SetDefaultDllDirectories);
		KxSysAPI_INIT_FUNCTION(Kernel32, AddDllDirectory);
		KxSysAPI_INIT_FUNCTION(Kernel32, RemoveDllDirectory);
		KxSysAPI_INIT_FUNCTION(Kernel32, SetDllDirectoryW);
		KxSysAPI_INIT_FUNCTION(Kernel32, GetDllDirectoryW);
	}

	if (KxSysAPI_CHECK_LIBRARY(DWMAPI))
	{
		KxSysAPI_INIT_FUNCTION(DWMAPI, DwmIsCompositionEnabled);
		KxSysAPI_INIT_FUNCTION(DWMAPI, DwmGetColorizationColor);
		KxSysAPI_INIT_FUNCTION(DWMAPI, DwmExtendFrameIntoClientArea);
		KxSysAPI_INIT_FUNCTION(DWMAPI, DwmEnableBlurBehindWindow);
	}

	if (KxSysAPI_CHECK_LIBRARY(DbgHelp))
	{
		KxSysAPI_INIT_FUNCTION(DbgHelp, ImageNtHeader);
	}
}
void KxSystemAPI::UnInitFunctions()
{
	KxSysAPI_UNLOAD_LIBRARY(NTDLL);
	KxSysAPI_UNLOAD_LIBRARY(Kernel32);
	KxSysAPI_UNLOAD_LIBRARY(DWMAPI);
	KxSysAPI_UNLOAD_LIBRARY(DbgHelp);
}

//////////////////////////////////////////////////////////////////////////
class KxSystemAPIModule: public wxModule
{
	public:
		virtual bool OnInit() override
		{
			KxSystemAPI::InitFunctions();
			return true;
		}
		virtual void OnExit() override
		{
			KxSystemAPI::UnInitFunctions();
		}

	private:
		wxDECLARE_DYNAMIC_CLASS(KxSystemAPIModule);
};
wxIMPLEMENT_DYNAMIC_CLASS(KxSystemAPIModule, wxModule);
