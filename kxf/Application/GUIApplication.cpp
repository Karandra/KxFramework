#include "stdafx.h"
#include "GUIApplication.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/System/SystemAppearance.h"
#include "kxf/Utility/Container.h"
#include <uxtheme.h>

namespace
{
	template<class TContainer>
	bool IsWindowInContainer(const TContainer& container, wxWindow& window)
	{
		return kxf::Utility::Contains(container, [&](const auto& item)
		{
			return item.get() == &window;
		});
	};
}

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
		wxInitializeStockLists();
		wxBitmap::InitStandardHandlers();
		m_LayoutDirection = GUIApplication::GetLayoutDirection();

		return ICoreApplication::OnCreate();
	}
	void GUIApplication::OnDestroy()
	{
		// Delete any remaining top level windows
		DeleteAllTopLevelWindows();

		// Undo everything we did in the 'OnCreate' above
		wxBitmap::CleanUpHandlers();
		wxStockGDI::DeleteAll();
		wxDeleteStockLists();
		wxDELETE(wxTheColourDatabase);

		ICoreApplication::OnDestroy();
	}
	int GUIApplication::OnRun()
	{
		// If the initial value hasn't been changed, use the default 'ExitOnLastFrameDelete::Always' from now on.
		if (m_ExitOnLastFrameDelete == ExitOnLastFrameDelete::Later)
		{
			m_ExitOnLastFrameDelete = ExitOnLastFrameDelete::Always;
		}

		return ICoreApplication::OnRun();
	}

	// Application::IActiveEventLoop
	bool GUIApplication::DispatchIdle()
	{
		// Send an event to the application instance itself first
		bool needMore = ICoreApplication::DispatchIdle();

		ReadLockGuard lock(m_ScheduledForDestructionLock);
		auto IsDestructionScheduled = [&](wxWindow& window)
		{
			return IsWindowInContainer(m_ScheduledForDestruction, window);
		};

		for (wxWindow* window: wxTopLevelWindows)
		{
			// Don't send idle events to the windows that are about to be destroyed anyhow, this is wasteful and unexpected.
			wxIdleEvent event;
			if (window && !IsDestructionScheduled(*window) && window->SendIdleEvents(event))
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
		// Ask the user about what to do: use the WinAPI function here as it could be dangerous to use any framework code in this state.
		// TODO: Use 'TaskDialog[Indirect]' instead.
		const auto result = ::MessageBoxW(nullptr,
										  wxS("An unhandled exception occurred. Press \"Abort\" to terminate the program,\r\n\"Retry\" to exit the program normally and \"Ignore\" to try to continue."),
										  wxS("Unhandled exception"),
										  MB_ABORTRETRYIGNORE|MB_ICONERROR|MB_TASKMODAL);
		switch (result)
		{
			case IDABORT:
			{
				throw;
			}
			case IDIGNORE:
			{
				return true;
			}
		};
		return false;
	}

	// IGUIApplication
	wxWindow* GUIApplication::GetTopWindow() const
	{
		auto IsDestructionScheduled = [&](wxWindow& window)
		{
			return IsWindowInContainer(m_ScheduledForDestruction, window);
		};
		ReadLockGuard lock(m_ScheduledForDestructionLock);

		// If there is no top window or it is about to be destroyed, we need to search for the first TLW which is not pending delete.
		if (m_TopWindow && !IsDestructionScheduled(*m_TopWindow))
		{
			return m_TopWindow;
		}
		else
		{
			for (wxWindow* window: wxTopLevelWindows)
			{
				if (window && !IsDestructionScheduled(*window))
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
				.Execute();
		}
	}

	UI::LayoutDirection GUIApplication::GetLayoutDirection() const
	{
		using namespace UI;

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
	void GUIApplication::SetLayoutDirection(UI::LayoutDirection direction)
	{
		using namespace UI;

		m_LayoutDirection = direction;
		::SetProcessDefaultLayout(direction ==LayoutDirection::RightToLeft ? LAYOUT_RTL : 0);
	}

	String GUIApplication::GetNativeTheme() const
	{
		if (System::IsWindows10OrGreater())
		{
			return SystemAppearance::IsDark() ? wxS("ModernUI Dark") : wxS("ModernUI Light");
		}
		else if (System::IsWindows8OrGreater())
		{
			return wxS("ModernUI");
		}
		else if (System::IsWindowsVistaOrGreater())
		{
			return ::IsAppThemed() ? wxS("Aero") : wxS("Classic");
		}
		else if (System::IsWindowsXPOrGreater())
		{
			return ::IsAppThemed() ? wxS("Luna") : wxS("Classic");
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
