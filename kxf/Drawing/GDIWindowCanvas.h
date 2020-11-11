#pragma once
#include "GDICanvas.h"
#include <wx/dcclient.h>

namespace kxf
{
	class KX_API GDIWindowCanvas final: public GDICanvas
	{
		private:
			wxWindowDC m_DC;

		public:
			GDIWindowCanvas(wxWindow& window)
				:GDICanvas(m_DC), m_DC(&window)
			{
			}
	};

	class KX_API GDIWindowClientCanvas final: public GDICanvas
	{
		private:
			wxClientDC m_DC;

		public:
			GDIWindowClientCanvas(wxWindow& window)
				:GDICanvas(m_DC), m_DC(&window)
			{
			}
	};

	class KX_API GDIWindowPaintCanvas final: public GDICanvas
	{
		private:
			wxPaintDC m_DC;

		public:
			GDIWindowPaintCanvas(wxWindow& window)
				:GDICanvas(m_DC), m_DC(&window)
			{
			}
	};
}
