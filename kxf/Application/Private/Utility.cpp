#include "KxfPCH.h"
#include "Utility.h"
#include "kxf/Core/Format.h"
#include <Windows.h>

namespace kxf::Application::Private
{
	void OnUnhandledException()
	{
		// We're called from an exception handler so we can re-throw the exception to recover its type
		String what;
		try
		{
			throw;
		}
		catch (std::exception& e)
		{
			what = Format("standard exception of type \"{}\" with message \"{}\"", typeid(e).name(), e.what());
		}
		catch (...)
		{
			what = "unknown exception";
		}
		wxMessageOutputBest().Printf("Unhandled %s; terminating %s.\n", what.wc_str(), wxIsMainThread() ? "the application" : "the thread in which it happened");
	}
	void OnFatalException()
	{
		// Nothing to do
	}

	bool OnMainLoopException()
	{
		throw;
	}
	bool OnMainLoopExceptionGUI()
	{
		// Ask the user about what to do: use the WinAPI function here as it could be dangerous to use any framework code in this state.
		// TODO: Use 'TaskDialog[Indirect]' instead.
		const auto result = ::MessageBoxW(nullptr,
										  L"An unhandled exception occurred. Press \"Abort\" to terminate the program,\r\n\"Retry\" to exit the program normally and \"Ignore\" to try to continue.",
										  L"Unhandled exception",
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
}
