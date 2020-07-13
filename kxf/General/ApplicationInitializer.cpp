#include "stdafx.h"
#include "ApplicationInitializer.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include <wx/init.h>
#include <wx/except.h>

namespace
{
	bool WinEntryStart(void* instance, void* prevInstance, char* commandLine, int showWindow)
	{
		return wxEntryStart(reinterpret_cast<HINSTANCE>(instance), reinterpret_cast<HINSTANCE>(prevInstance), commandLine, showWindow != -1 ? showWindow : SW_SHOWNORMAL);
	}
}

namespace kxf
{
	bool ApplicationInitializer::OnInitCommon()
	{
		wxDISABLE_DEBUG_SUPPORT();
		if (wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, m_App.GetDisplayName().c_str()))
		{
			// This will tell 'wxInitialize' to use already existing application instance instead of attempting to create a new one
			wxAppConsole::SetInstance(&m_App.GetWxImpl());
			wxAppConsole::SetInitializerFunction(nullptr);

			return true;
		}
		return false;
	}
	bool ApplicationInitializer::OnInit()
	{
		return wxInitialize();
	}
	bool ApplicationInitializer::OnInit(int argc, char** argv)
	{
		m_App.InitializeCommandLine(argv, static_cast<size_t>(argc));
		return wxInitialize(argc, argv);
	}
	bool ApplicationInitializer::OnInit(int argc, wchar_t** argv)
	{
		m_App.InitializeCommandLine(argv, static_cast<size_t>(argc));
		return wxInitialize(argc, argv);
	}
	void ApplicationInitializer::OnInitDone()
	{
		/// Initialization is done, we can assign ICoreApplication instance now
		ICoreApplication::SetInstance(&m_App);
	}
	
	void ApplicationInitializer::OnTerminate()
	{
		if (m_IsCreated)
		{
			m_App.OnDestroy();
			m_IsCreated = false;
		}

		if (m_IsInitializedCommon)
		{
			// Reset application object pointer prior to calling 'wxUninitialize' as it'll try to call C++ 'operator delete' on its instance.
			wxAppConsole::SetInstance(nullptr);

			if (m_IsInitialized)
			{
				wxUninitialize();
				m_IsInitialized = false;

				// Reset ICoreApplication instance
				ICoreApplication::SetInstance(nullptr);
			}
			m_IsInitializedCommon = false;
		}
	}

	int ApplicationInitializer::Run() noexcept
	{
		try
		{
			if (m_App.OnInit())
			{
				// Ensure that OnExit() is called if OnInit() had succeeded
				Utility::CallAtScopeExit callOnExit = [&]()
				{
					m_App.OnExit();
				};
				
				// Run the main loop
				return m_App.OnRun();
			}

			// Don't call OnExit() if OnInit() failed
			return m_App.GetExitCode().value_or(-1);
		}
		catch (...)
		{
			m_App.OnUnhandledException();
			return m_App.GetExitCode().value_or(-1);
		}
	}
}
