#pragma once
#include "../Common.h"

namespace kxf::Application::Private
{
	void OnUnhandledException();
	void OnFatalException();

	bool OnMainLoopException();
	bool OnMainLoopExceptionGUI();
}
