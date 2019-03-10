#pragma once
#include "KxFramework/KxFramework.h"

class KxGCInterpolationQualityChanger final
{
	private:
		wxGraphicsContext& m_GC;
		wxInterpolationQuality m_OriginalQuality = wxINTERPOLATION_NONE;

	public:
		KxGCInterpolationQualityChanger(wxGraphicsContext& gc, wxInterpolationQuality newQuality)
			:m_GC(gc)
		{
			m_OriginalQuality = m_GC.GetInterpolationQuality();
			m_GC.SetInterpolationQuality(newQuality);
		}
		~KxGCInterpolationQualityChanger()
		{
			m_GC.SetInterpolationQuality(m_OriginalQuality);
		}
};
