#pragma once
#include "GDICanvas.h"
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

namespace kxf
{
	enum GDIBufferedCanvasFlag: uint32_t
	{
		None = 0,

		BufferVirtualArea = wxBUFFER_VIRTUAL_AREA,
		BufferClientArea = wxBUFFER_CLIENT_AREA,
		VirtualArea = wxBUFFER_USES_SHARED_BUFFER,
	};
	KxFlagSet_Declare(GDIBufferedCanvasFlag);
}

namespace kxf::Drawing
{
	class KX_API GDIMemoryCanvasBase: public GDICanvas
	{
		protected:
			const wxMemoryDC& GetMemoryDC() const noexcept
			{
				return static_cast<wxMemoryDC&>(*m_DC);
			}
			wxMemoryDC& GetMemoryDC() noexcept
			{
				return static_cast<wxMemoryDC&>(*m_DC);
			}

		public:
			GDIMemoryCanvasBase() = default;
			GDIMemoryCanvasBase(wxMemoryDC& other)
				:GDICanvas(other)
			{
			}
			GDIMemoryCanvasBase(const GDIMemoryCanvasBase&) = delete;

		public:
			// GDIMemoryCanvasBase
			const wxMemoryDC& ToWxDC() const noexcept
			{
				return GetMemoryDC();
			}
			wxMemoryDC& ToWxDC() noexcept
			{
				return GetMemoryDC();
			}

			Bitmap GetSelectedBitmap() const
			{
				return GetMemoryDC().GetSelectedBitmap();
			}
			void SelectObject(Bitmap& bitmap)
			{
				GetMemoryDC().SelectObject(bitmap.ToWxBitmap());
			}
			void SelectObjectAsSource(const Bitmap& bitmap)
			{
				GetMemoryDC().SelectObjectAsSource(bitmap.ToWxBitmap());
			}
			void UnselectObject()
			{
				GetMemoryDC().SelectObject(wxNullBitmap);
			}

		public:
			GDICanvas& operator=(const GDICanvas&) = delete;
	};

	class KX_API GDIBufferedCanvasBase: public GDIMemoryCanvasBase
	{
		protected:
			const wxBufferedDC& GetBufferedDC() const noexcept
			{
				return static_cast<wxBufferedDC&>(*m_DC);
			}
			wxBufferedDC& GetBufferedDC() noexcept
			{
				return static_cast<wxBufferedDC&>(*m_DC);
			}

		public:
			GDIBufferedCanvasBase() = default;
			GDIBufferedCanvasBase(wxBufferedDC& other)
				:GDIMemoryCanvasBase(other)
			{
			}
			GDIBufferedCanvasBase(const GDIMemoryCanvasBase&) = delete;

		public:
			// GDIMemoryCanvasBase
			const wxBufferedDC& ToWxDC() const noexcept
			{
				return GetBufferedDC();
			}
			wxBufferedDC& ToWxDC() noexcept
			{
				return GetBufferedDC();
			}

			FlagSet<GDIBufferedCanvasFlag> GetFlags() const
			{
				return static_cast<GDIBufferedCanvasFlag>(GetBufferedDC().GetStyle());
			}
			void SetFlags(FlagSet<GDIBufferedCanvasFlag> flags)
			{
				GetBufferedDC().SetStyle(flags.ToInt<int>());
			}

			void UnMask()
			{
				GetBufferedDC().UnMask();
			}

		public:
			GDICanvas& operator=(const GDICanvas&) = delete;
	};
}

namespace kxf
{
	class KX_API GDIMemoryCanvas final: public Drawing::GDIMemoryCanvasBase
	{
		private:
			wxMemoryDC m_DC;

		public:
			GDIMemoryCanvas()
				:GDIMemoryCanvasBase(m_DC)
			{
			}
			GDIMemoryCanvas(const GDICanvas& other)
				:GDIMemoryCanvasBase(m_DC), m_DC(const_cast<wxDC*>(&other.ToWxDC()))
			{
			}
			GDIMemoryCanvas(Bitmap& bitmap)
				:GDIMemoryCanvasBase(m_DC), m_DC(bitmap.ToWxBitmap())
			{
			}
	};

	class KX_API GDIBufferedCanvas final: public Drawing::GDIBufferedCanvasBase
	{
		private:
			wxBufferedDC m_DC;

		public:
			GDIBufferedCanvas(GDICanvas& other, const Size& size, FlagSet<GDIBufferedCanvasFlag> flags)
				:GDIBufferedCanvasBase(m_DC), m_DC(&other.ToWxDC(), size, flags.ToInt<int>())
			{
			}
			GDIBufferedCanvas(GDICanvas& other, Bitmap& bitmap, FlagSet<GDIBufferedCanvasFlag> flags)
				:GDIBufferedCanvasBase(m_DC), m_DC(&other.ToWxDC(), bitmap.ToWxBitmap(), flags.ToInt<int>())
			{
			}
	};

	class KX_API GDIBufferedPaintCanvas final: public Drawing::GDIBufferedCanvasBase
	{
		private:
			wxBufferedPaintDC m_DC;

		public:
			GDIBufferedPaintCanvas(wxWindow& window, FlagSet<GDIBufferedCanvasFlag> flags)
				:GDIBufferedCanvasBase(m_DC), m_DC(&window, flags.ToInt<int>())
			{
			}
			GDIBufferedPaintCanvas(wxWindow& window, Bitmap& bitmap, FlagSet<GDIBufferedCanvasFlag> flags)
				:GDIBufferedCanvasBase(m_DC), m_DC(&window, bitmap.ToWxBitmap(), flags.ToInt<int>())
			{
			}

		public:
			const wxBufferedPaintDC& ToWxDC() const noexcept
			{
				return m_DC;
			}
			wxBufferedPaintDC& ToWxDC() noexcept
			{
				return m_DC;
			}
	};

	class KX_API GDIAutoBufferedPaintCanvas final: public Drawing::GDIBufferedCanvasBase
	{
		private:
			wxAutoBufferedPaintDC m_DC;

		public:
			GDIAutoBufferedPaintCanvas(wxWindow& window)
				:GDIBufferedCanvasBase(m_DC), m_DC(&window)
			{
			}

		public:
			const wxAutoBufferedPaintDC& ToWxDC() const noexcept
			{
				return m_DC;
			}
			wxAutoBufferedPaintDC& ToWxDC() noexcept
			{
				return m_DC;
			}
	};
}
