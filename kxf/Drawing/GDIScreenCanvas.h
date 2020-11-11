#pragma once
#include "GDICanvas.h"
#include <wx/dcscreen.h>

namespace kxf
{
	class KX_API GDIScreenCanvas final: public GDICanvas
	{
		private:
			wxScreenDC m_DC;

		public:
			GDIScreenCanvas()
				:GDICanvas(m_DC)
			{
			}
	};
}
