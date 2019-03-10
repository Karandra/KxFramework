#pragma once
#include "KxFramework/KxFramework.h"

class KxGCAntialiasModeChanger final
{
	private:
		wxGraphicsContext& m_GC;
		wxAntialiasMode m_OriginalMode = wxANTIALIAS_NONE;

	public:
		KxGCAntialiasModeChanger(wxGraphicsContext& gc, wxAntialiasMode newMode)
			:m_GC(gc)
		{
			m_OriginalMode = m_GC.GetAntialiasMode();
			m_GC.SetAntialiasMode(newMode);
		}
		~KxGCAntialiasModeChanger()
		{
			m_GC.SetAntialiasMode(m_OriginalMode);
		}
};
