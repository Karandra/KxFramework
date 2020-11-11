#pragma once
#include "GDICanvas.h"
#include <wx/dcmirror.h>

namespace kxf
{
	class KX_API GDIMirroringCanvas final: public GDICanvas
	{
		private:
			wxMirrorDC m_DC;

		public:
			GDIMirroringCanvas(GDICanvas& other, bool enableMirroring)
				:GDICanvas(m_DC), m_DC(other.ToWxDC(), enableMirroring)
			{
			}
	};
}
