#pragma once
#include "Common.h"
#include "Events/ThreadEvent.h"
#include "Kx/Core/EventSystem/EvtHandler.h"
#include <wx/thread.h>

class KX_API KxThread: public KxEvtHandler, public wxThread
{
	protected:
		ExitCode Entry() override;

	public:
		KxThread(wxThreadKind kind = wxThreadKind::wxTHREAD_JOINABLE)
			:wxThread(kind)
		{
		}
};
