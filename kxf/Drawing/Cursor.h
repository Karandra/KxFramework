#pragma once
#include "Common.h"
#include "IGDIImage.h"
#include <wx/cursor.h>

namespace kxf::Drawing
{
	enum class StockCursor
	{
		None = wxCURSOR_NONE,
		Default = wxCURSOR_DEFAULT,

		ArrowLeft = wxCURSOR_ARROW,
		ArrowRight = wxCURSOR_RIGHT_ARROW,
		ArrowWait = wxCURSOR_ARROWWAIT,
		ArrowQuestion = wxCURSOR_QUESTION_ARROW,

		PointLeft = wxCURSOR_POINT_LEFT,
		PointRight = wxCURSOR_POINT_RIGHT,

		LeftButton = wxCURSOR_LEFT_BUTTON,
		MiddleButton = wxCURSOR_MIDDLE_BUTTON,
		MiddleRight = wxCURSOR_RIGHT_BUTTON,

		SizeAny = wxCURSOR_SIZING,
		SizeRightTopLeftBottom = wxCURSOR_SIZENESW,
		SizeLeftTopRightBottom = wxCURSOR_SIZENWSE,
		SizeTopBottom = wxCURSOR_SIZENS,
		SizeLeftRight = wxCURSOR_SIZEWE,

		Char = wxCURSOR_CHAR,
		Cross = wxCURSOR_CROSS,
		Hand = wxCURSOR_HAND,
		NoEntry = wxCURSOR_NO_ENTRY,
		Magnifier = wxCURSOR_MAGNIFIER,
		Wait = wxCURSOR_WAIT,
		Watch = wxCURSOR_WATCH,
		IBeam = wxCURSOR_IBEAM,
		Bullseye = wxCURSOR_BULLSEYE,
		PaintPrush = wxCURSOR_PAINT_BRUSH,
		SprayCan = wxCURSOR_SPRAYCAN,
		Pencil = wxCURSOR_PENCIL,
		Blank = wxCURSOR_BLANK,
	};
}

namespace kxf
{
	class KX_API Cursor: public RTTI::ExtendInterface<Cursor, IGDIImage>
	{
		KxRTTI_DeclareIID(Cursor, {0xec12b28a, 0x111e, 0x4f00, {0x8c, 0xe0, 0xdd, 0xb, 0x18, 0x9, 0xf7, 0x5e}});

		private:
			wxCursor m_Cursor;
			Point m_HotSpot = Point::UnspecifiedPosition();

		public:
			Cursor() = default;
			Cursor(const wxCursor& other)
				:m_Cursor(other)
			{
			}

			Cursor(const Icon& other);
			Cursor(const Bitmap& other);
			Cursor(const Image& other);
			Cursor(const Cursor& other)
				:m_Cursor(other.m_Cursor), m_HotSpot(other.m_HotSpot)
			{
			}

			virtual ~Cursor() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Cursor.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return m_Cursor.GetHandle() == other.GetHandle();
			}
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return std::make_unique<Cursor>(m_Cursor);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// IGDIImage
			Size GetSize() const override
			{
				return m_Cursor.IsOk() ? Size(m_Cursor.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetDepth() const override
			{
				return m_Cursor.GetDepth();
			}

			bool Load(IInputStream& stream, Point hotSpot, ImageFormat format = ImageFormat::Any);
			bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) override
			{
				return Load(stream, Point::UnspecifiedPosition(), format);
			}
			bool Save(IOutputStream& stream, ImageFormat format) const override;

			// Cursor
			const wxCursor& ToWxCursor() const noexcept
			{
				return m_Cursor;
			}
			wxCursor& ToWxCursor() noexcept
			{
				return m_Cursor;
			}

			Bitmap ToBitmap() const;
			Image ToImage() const;
			Icon ToIcon() const;

			Point GetHotSpot() const
			{
				return m_Cursor.GetHotSpot();
			}
			void SetHotSpot(Point hotSpot)
			{
				// TODO: Update the hotspot on the cursor in memory
				m_HotSpot = std::move(hotSpot);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			Cursor& operator=(const Cursor& other)
			{
				m_Cursor = other.m_Cursor;
				m_HotSpot = other.m_HotSpot;

				return *this;
			}
	};
}

namespace kxf::Drawing
{
	inline constexpr wxStockCursor ToWxStockCursor(StockCursor cursorType) noexcept
	{
		return static_cast<wxStockCursor>(cursorType);
	}
	inline constexpr StockCursor FromWxStockCursor(wxStockCursor cursorType) noexcept
	{
		return static_cast<StockCursor>(cursorType);
	}

	Cursor GetStockCursor(Drawing::StockCursor cursor);
}
