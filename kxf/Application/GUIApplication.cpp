#include "KxfPCH.h"
#include "GUIApplication.h"
#include "Private/Utility.h"
#include "Private/NativeApp.h"
#include "kxf/EventSystem/Private/Win32GUIEventLoop.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/System/SystemAppearance.h"
#include "kxf/wxWidgets/Application.h"
#include "kxf/Utility/Container.h"
#include <uxtheme.h>

namespace kxf
{
	void GUIApplication::DeleteAllTopLevelWindows()
	{
		// TLWs remove themselves from 'wxTopLevelWindows' when destroyed, so iterate until none are left.
		while (!wxTopLevelWindows.empty())
		{
			// Do not use 'Destroy' here as it only puts the TLW in pending list but we want to delete them now.
			delete wxTopLevelWindows.GetFirst()->GetData();
		}
	}

	// ICoreApplication
	bool GUIApplication::OnCreate()
	{
		m_LayoutDirection = GUIApplication::GetLayoutDirection();

		if (auto app = Application::Private::NativeApp::GetInstance())
		{
			int argc = m_ArgC;
			m_NativeAppInitialized = app->wxApp::Initialize(argc, m_ArgVW);

			if (!m_NativeAppInitialized)
			{
				return false;
			}
		}

		return CoreApplication::OnCreate();
	}
	void GUIApplication::OnDestroy()
	{
		if (m_NativeAppInitialized)
		{
			if (auto app = Application::Private::NativeApp::GetInstance())
			{
				app->wxApp::CleanUp();
				m_NativeAppCleanedUp = true;
			}
		}
		CoreApplication::OnDestroy();
	}
	int GUIApplication::OnRun()
	{
		// If the initial value hasn't been changed, use the default 'ExitOnLastFrameDelete::Always' from now on.
		if (m_ExitOnLastFrameDelete == ExitOnLastFrameDelete::Later)
		{
			m_ExitOnLastFrameDelete = ExitOnLastFrameDelete::Always;

			// Notify the wxWidgets application
			if (auto app = Application::Private::NativeApp::GetInstance())
			{
				app->SetExitOnFrameDelete(true);
			}
		}

		// Ru the main loop
		return CoreApplication::OnRun();
	}

	// Application::IMainEventLoop
	std::unique_ptr<IEventLoop> GUIApplication::CreateMainLoop()
	{
		return std::make_unique<EventSystem::Private::Win32GUIEventLoop>();
	}

	// Application::IActiveEventLoop
	bool GUIApplication::DispatchIdle()
	{
		// Send an event to the application instance itself first
		bool needMore = CoreApplication::DispatchIdle();
		if (auto nativeApp = Application::Private::NativeApp::GetInstance())
		{
			if (nativeApp->wxApp::ProcessIdle())
			{
				needMore = true;
			}
		}

		ReadLockGuard lock(m_ScheduledForDestructionLock);
		for (wxWindow* window: wxTopLevelWindows)
		{
			// Don't send idle events to the windows that are about to be destroyed anyhow, this is wasteful and unexpected.
			wxIdleEvent event;
			if (window && !IsScheduledForDestruction(*window) && window->SendIdleEvents(event))
			{
				needMore = true;
			}
		}

		wxUpdateUIEvent::ResetUpdateTime();
		return needMore;
	}

	// Application::IExceptionHandler
	bool GUIApplication::OnMainLoopException()
	{
		return Application::Private::OnMainLoopExceptionGUI();
	}

	// IGUIApplication
	wxWindow* GUIApplication::GetTopWindow() const
	{
		ReadLockGuard lock(m_ScheduledForDestructionLock);

		// If there is no top window or it is about to be destroyed, we need to search for the first TLW which is not pending delete.
		if (m_TopWindow && !IsScheduledForDestruction(*m_TopWindow))
		{
			return m_TopWindow;
		}
		else
		{
			for (wxWindow* window: wxTopLevelWindows)
			{
				if (window && !IsScheduledForDestruction(*window))
				{
					return window;
				}
			}
		}
		return nullptr;
	}
	void GUIApplication::SetTopWindow(wxWindow* window)
	{
		m_TopWindow = window;
		if (auto app = Application::Private::NativeApp::GetInstance())
		{
			app->SetTopWindow(window);
		}
	}

	bool GUIApplication::ShoudExitOnLastFrameDelete() const
	{
		return m_ExitOnLastFrameDelete == ExitOnLastFrameDelete::Always;
	}
	void GUIApplication::ExitOnLastFrameDelete(bool enable)
	{
		m_ExitOnLastFrameDelete = enable ? ExitOnLastFrameDelete::Always : ExitOnLastFrameDelete::Never;
		if (auto app = Application::Private::NativeApp::GetInstance())
		{
			app->SetExitOnFrameDelete(enable);
		}
	}

	bool GUIApplication::IsActive() const
	{
		return m_IsActive;
	}
	void GUIApplication::SetActive(bool active, wxWindow* window)
	{
		if (m_IsActive != active)
		{
			m_IsActive = active;

			BuildProcessEvent(ICoreApplication::EvtActivated, active, ActivateEventReason::Unknown)
				.SetSourceToSelf()
				.Process();
		}
	}

	LayoutDirection GUIApplication::GetLayoutDirection() const
	{
		if (m_LayoutDirection != LayoutDirection::Default)
		{
			return m_LayoutDirection;
		}
		else if (const wxLocale* locale = wxGetLocale())
		{
			if (const wxLanguageInfo* info = wxLocale::GetLanguageInfo(locale->GetLanguage()))
			{
				switch (info->LayoutDirection)
				{
					case wxLayout_LeftToRight:
					{
						return LayoutDirection::LeftToRight;
					}
					case wxLayout_RightToLeft:
					{
						return LayoutDirection::RightToLeft;
					}
				};
			}

			DWORD layoutDirection = 0;
			if (::GetProcessDefaultLayout(&layoutDirection))
			{
				return layoutDirection == LAYOUT_RTL ? LayoutDirection::RightToLeft : LayoutDirection::LeftToRight;
			}
		}

		// We don't know
		return LayoutDirection::Default;
	}
	void GUIApplication::SetLayoutDirection(LayoutDirection direction)
	{
		m_LayoutDirection = direction;
		::SetProcessDefaultLayout(direction == LayoutDirection::RightToLeft ? LAYOUT_RTL : 0);
	}

	String GUIApplication::GetNativeTheme() const
	{
		if (System::IsWindows10OrGreater())
		{
			return SystemAppearance::IsDark() ? "ModernUI Dark" : "ModernUI Light";
		}
		else if (System::IsWindows8OrGreater())
		{
			return "ModernUI";
		}
		else if (System::IsWindowsVistaOrGreater())
		{
			return ::IsAppThemed() ? "Aero" : "Classic";
		}
		else if (System::IsWindowsXPOrGreater())
		{
			return ::IsAppThemed() ? "Luna" : "Classic";
		}
		return {};
	}
	bool GUIApplication::SetNativeTheme(const String& themeName)
	{
		return false;
	}

	bool GUIApplication::Yield(wxWindow& window, FlagSet<EventYieldFlag> flags)
	{
		wxWindowDisabler windowDisabler(&window);

		IEventLoop* activeLoop = GetActiveEventLoop();
		return activeLoop && activeLoop->Yield(flags);
	}
	bool GUIApplication::YieldFor(wxWindow& window, FlagSet<EventCategory> toProcess)
	{
		wxWindowDisabler windowDisabler(&window);

		IEventLoop* activeLoop = GetActiveEventLoop();
		return activeLoop && activeLoop->YieldFor(toProcess);
	}
}
