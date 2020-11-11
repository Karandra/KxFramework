#pragma once
#include "GDIContext.h"
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

namespace kxf
{
	enum GDIBufferedContextFlag: uint32_t
	{
		None = 0,

		BufferVirtualArea = wxBUFFER_VIRTUAL_AREA,
		BufferClientArea = wxBUFFER_CLIENT_AREA,
		VirtualArea = wxBUFFER_USES_SHARED_BUFFER,
	};
	KxFlagSet_Declare(GDIBufferedContextFlag);
}

namespace kxf::Drawing
{
	class KX_API GDIMemoryContextBase: public GDIContext
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
			GDIMemoryContextBase() = default;
			GDIMemoryContextBase(wxMemoryDC& other)
				:GDIContext(other)
			{
			}
			GDIMemoryContextBase(const GDIMemoryContextBase&) = delete;

		public:
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
			GDIContext& operator=(const GDIContext&) = delete;
	};

	class KX_API GDIBufferedContextBase: public GDIMemoryContextBase
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
			GDIBufferedContextBase() = default;
			GDIBufferedContextBase(wxBufferedDC& other)
				:GDIMemoryContextBase(other)
			{
			}
			GDIBufferedContextBase(const GDIMemoryContextBase&) = delete;

		public:
			const wxBufferedDC& ToWxDC() const noexcept
			{
				return GetBufferedDC();
			}
			wxBufferedDC& ToWxDC() noexcept
			{
				return GetBufferedDC();
			}

			FlagSet<GDIBufferedContextFlag> GetFlags() const
			{
				return static_cast<GDIBufferedContextFlag>(GetBufferedDC().GetStyle());
			}
			void SetFlags(FlagSet<GDIBufferedContextFlag> flags)
			{
				GetBufferedDC().SetStyle(flags.ToInt<int>());
			}

			void UnMask()
			{
				GetBufferedDC().UnMask();
			}

		public:
			GDIContext& operator=(const GDIContext&) = delete;
	};
}

namespace kxf
{
	class KX_API GDIMemoryContext final: public Drawing::GDIMemoryContextBase
	{
		private:
			wxMemoryDC m_DC;

		public:
			GDIMemoryContext()
				:GDIMemoryContextBase(m_DC)
			{
			}
			GDIMemoryContext(const GDIContext& other)
				:GDIMemoryContextBase(m_DC), m_DC(const_cast<wxDC*>(&other.ToWxDC()))
			{
			}
			GDIMemoryContext(Bitmap& bitmap)
				:GDIMemoryContextBase(m_DC), m_DC(bitmap.ToWxBitmap())
			{
			}
	};

	class KX_API GDIBufferedContext final: public Drawing::GDIBufferedContextBase
	{
		private:
			wxBufferedDC m_DC;

		public:
			GDIBufferedContext(GDIContext& other, const Size& size, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&other.ToWxDC(), size, flags.ToInt<int>())
			{
			}
			GDIBufferedContext(GDIContext& other, Bitmap& bitmap, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&other.ToWxDC(), bitmap.ToWxBitmap(), flags.ToInt<int>())
			{
			}
	};

	class KX_API GDIBufferedPaintContext final: public Drawing::GDIBufferedContextBase
	{
		private:
			wxBufferedPaintDC m_DC;

		public:
			GDIBufferedPaintContext(wxWindow& window, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&window, flags.ToInt<int>())
			{
			}
			GDIBufferedPaintContext(wxWindow& window, Bitmap& bitmap, FlagSet<GDIBufferedContextFlag> flags)
				:GDIBufferedContextBase(m_DC), m_DC(&window, bitmap.ToWxBitmap(), flags.ToInt<int>())
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

	class KX_API GDIAutoBufferedPaintContext final: public Drawing::GDIBufferedContextBase
	{
		private:
			wxAutoBufferedPaintDC m_DC;

		public:
			GDIAutoBufferedPaintContext(wxWindow& window)
				:GDIBufferedContextBase(m_DC), m_DC(&window)
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
