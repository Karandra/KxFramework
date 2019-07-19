#pragma once
#include "Common.h"
#include "Events/ThreadEvent.h"
#include <wx/thread.h>

class KX_API KxThread: public wxThread, public KxEvtHandler
{
	private:
		wxEvtHandler m_EvtHandler;

	protected:
		ExitCode Entry() override;

	public:
		KxThread(wxThreadKind kind = wxThreadKind::wxTHREAD_JOINABLE)
			:wxThread(kind), KxEvtHandler(m_EvtHandler)
		{
		}
};
