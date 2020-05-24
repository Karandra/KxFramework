#pragma once
#include "Common.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "ThreadEvent.h"
#include <wx/thread.h>

namespace kxf
{
	class KX_API JoinableThread: public EvtHandler, public wxThread
	{
		protected:
			ExitCode Entry() override;

		public:
			JoinableThread()
				:wxThread(wxThreadKind::wxTHREAD_JOINABLE)
			{
			}
	};
}
