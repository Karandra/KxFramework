#pragma once
#include "GDICanvas.h"
#include <wx/dcclient.h>

namespace kxf::Drawing
{
	class KX_API GDIWindowCanvasBase: public GDICanvas
	{
		protected:
			const wxWindowDC& GetWindowDC() const noexcept
			{
				return static_cast<wxWindowDC&>(*m_DC);
			}
			wxWindowDC& GetWindowDC() noexcept
			{
				return static_cast<wxWindowDC&>(*m_DC);
			}

		public:
			GDIWindowCanvasBase() = default;
			GDIWindowCanvasBase(wxWindowDC& other)
				:GDICanvas(other)
			{
			}
			GDIWindowCanvasBase(const GDIWindowCanvasBase&) = delete;

		public:
			// GDIWindowCanvasBase
			const wxWindowDC& ToWxDC() const noexcept
			{
				return GetWindowDC();
			}
			wxWindowDC& ToWxDC() noexcept
			{
				return GetWindowDC();
			}

		public:
			GDICanvas& operator=(const GDICanvas&) = delete;
	};
}

namespace kxf
{
	class KX_API GDIWindowCanvas final: public Drawing::GDIWindowCanvasBase
	{
		private:
			wxWindowDC m_DC;

		public:
			GDIWindowCanvas(wxWindow& window)
				:GDIWindowCanvasBase(m_DC), m_DC(&window)
			{
			}

	};

	class KX_API GDIWindowClientCanvas final: public Drawing::GDIWindowCanvasBase
	{
		private:
			wxClientDC m_DC;

		public:
			GDIWindowClientCanvas(wxWindow& window)
				:GDIWindowCanvasBase(m_DC), m_DC(&window)
			{
			}

		public:
			const wxClientDC& ToWxDC() const noexcept
			{
				return m_DC;
			}
			wxClientDC& ToWxDC() noexcept
			{
				return m_DC;
			}
	};

	class KX_API GDIWindowPaintCanvas final: public Drawing::GDIWindowCanvasBase
	{
		private:
			wxPaintDC m_DC;

		public:
			GDIWindowPaintCanvas(wxWindow& window)
				:GDIWindowCanvasBase(m_DC), m_DC(&window)
			{
			}

		public:
			const wxPaintDC& ToWxDC() const noexcept
			{
				return m_DC;
			}
			wxPaintDC& ToWxDC() noexcept
			{
				return m_DC;
			}
	};
}
