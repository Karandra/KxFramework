#include "KxStdAfx.h"
#include "FocusEvent.h"

namespace KxSciter
{
	KxEVENT_DEFINE_LOCAL(FocusEvent, SetFocus);
	KxEVENT_DEFINE_LOCAL(FocusEvent, KillFocus);
	KxEVENT_DEFINE_LOCAL(FocusEvent, ContainerSetFocus);
	KxEVENT_DEFINE_LOCAL(FocusEvent, ContainerKillFocus);
	KxEVENT_DEFINE_LOCAL(FocusEvent, Request);
}
