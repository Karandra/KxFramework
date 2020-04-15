#include "KxStdAfx.h"
#include "BasicApplication.h"

namespace KxFramework::Private
{
	bool ParseCommandLine(wxAppConsole& app, wxCmdLineParser& parser)
	{
		parser.SetCmdLine(app.argc, app.argv);
		return parser.Parse() == 0;
	}
}
