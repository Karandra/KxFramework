#include "KxStdAfx.h"
#include "KxFramework/KxApp.h"

bool KxAppInternal::ParseCommandLine(wxAppConsole* app, wxCmdLineParser& parser)
{
	parser.SetCmdLine(app->argc, app->argv);
	return parser.Parse() == 0;
}
