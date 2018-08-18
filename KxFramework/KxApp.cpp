#include "KxStdAfx.h"
#include "KxFramework/KxApp.h"

bool KxAppInternal::ParseCommandLine(wxCmdLineParser& parser)
{
	parser.SetCmdLine(wxTheApp->argc, wxTheApp->argv);
	return parser.Parse() == 0;
}
