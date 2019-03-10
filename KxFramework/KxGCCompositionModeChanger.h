#pragma once
#include "KxFramework/KxFramework.h"

class KxGCCompositionModeChanger final
{
	private:
		wxGraphicsContext& m_GC;
		wxCompositionMode m_OriginalMode = wxCOMPOSITION_INVALID;

	public:
		KxGCCompositionModeChanger(wxGraphicsContext& gc, wxCompositionMode newMode)
			:m_GC(gc)
		{
			m_OriginalMode = m_GC.GetCompositionMode();
			m_GC.SetCompositionMode(newMode);
		}
		~KxGCCompositionModeChanger()
		{
			m_GC.SetCompositionMode(m_OriginalMode);
		}
};
