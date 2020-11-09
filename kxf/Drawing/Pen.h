#pragma once
#include "Common.h"
#include "Color.h"
#include "Bitmap.h"
#include "IGDIObject.h"
#include <wx/brush.h>

namespace kxf
{
	enum class PenStyle
	{
		None = wxPENSTYLE_INVALID,

		Solid = wxPENSTYLE_SOLID,
		Stipple = wxPENSTYLE_STIPPLE,
		Transparent = wxBRUSHSTYLE_TRANSPARENT,

		Dot = wxPENSTYLE_DOT,
		DotDash = wxPENSTYLE_DOT_DASH,
		ShortDash = wxPENSTYLE_SHORT_DASH,
		LongDash = wxPENSTYLE_LONG_DASH,
		UserDash = wxPENSTYLE_USER_DASH,

		HatchVertical = wxPENSTYLE_VERTICAL_HATCH,
		HatchHorizontal = wxPENSTYLE_HORIZONTAL_HATCH,
		HatchCross = wxPENSTYLE_CROSS_HATCH,
		HatchCrossDiagonal = wxPENSTYLE_CROSSDIAG_HATCH,
		HatchForwardDiagonal = wxPENSTYLE_FDIAGONAL_HATCH,
		HatchBackwardDiagonal = wxPENSTYLE_BDIAGONAL_HATCH,
	};
	enum class PenCap
	{
		None = wxCAP_INVALID,
		Flat = wxCAP_BUTT,
		Round = wxCAP_ROUND,
		Square = wxCAP_PROJECTING,
	};
	enum class PenJoin
	{
		None = wxJOIN_INVALID,

		Bevel = wxJOIN_BEVEL,
		Miter = wxJOIN_MITER,
		Round = wxJOIN_ROUND,
	};

	enum class StockPen
	{
		Black,
		BlackDashed,
		Cyan,
		Blue,
		Red,
		Green,
		Yellow,
		White,
		Gray,
		LightGray,
		MediumGray,
		Transparent
	};
}

namespace kxf
{
	class KX_API Pen: public RTTI::ExtendInterface<Pen, IGDIObject>
	{
		KxRTTI_DeclareIID(Pen, {0xc5cfc8bc, 0x1f1f, 0x4c5e, {0xbb, 0x51, 0x8, 0xb6, 0x9d, 0xc2, 0xa0, 0x7c}});

		public:
			using Dash = wxDash;

		private:
			wxPen m_Pen;

		public:
			Pen() = default;
			Pen(const wxPen& other)
				:m_Pen(other)
			{
			}
			Pen(const Pen& other)
				:m_Pen(other.m_Pen)
			{
			}
			Pen(const Color& color, PenStyle style)
				:m_Pen(color.ToWxColor(), static_cast<wxBrushStyle>(style))
			{
			}
			Pen(const Bitmap& stippleBitmap, int width)
				:m_Pen(stippleBitmap.ToWxBitmap(), width)
			{
			}
			virtual ~Pen()
			{
				m_Pen.SetDashes(0, nullptr);
			}

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Pen.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<Pen>())
				{
					return m_Pen == object->m_Pen;
				}
				return false;
			}
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return std::make_unique<Pen>(m_Pen);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// Pen
			const wxPen& ToWxPen() const noexcept
			{
				return m_Pen;
			}
			wxPen& ToWxPen() noexcept
			{
				return m_Pen;
			}

			Color GetColor() const
			{
				return m_Pen.GetColour();
			}
			void SetColor(const Color& color)
			{
				m_Pen.SetColour(color.ToWxColor());
			}

			PenStyle GetStyle() const
			{
				return static_cast<PenStyle>(m_Pen.GetStyle());
			}
			void SetStyle(PenStyle style)
			{
				m_Pen.SetStyle(static_cast<wxPenStyle>(style));
			}
			bool IsTransparent() const
			{
				return m_Pen.IsTransparent();
			}

			PenJoin GetJoin() const
			{
				return static_cast<PenJoin>(m_Pen.GetJoin());
			}
			void SetJoin(PenJoin join)
			{
				m_Pen.SetJoin(static_cast<wxPenJoin>(join));
			}

			PenCap GetCap() const
			{
				return static_cast<PenCap>(m_Pen.GetCap());
			}
			void SetJoin(PenCap cap)
			{
				m_Pen.SetCap(static_cast<wxPenCap>(cap));
			}

			Bitmap GetStipple() const
			{
				const wxBitmap* stipple = m_Pen.GetStipple();
				if (stipple && stipple->IsOk())
				{
					return *stipple;
				}
				return {};
			}
			void SetStipple(const Bitmap& stipple)
			{
				m_Pen.SetStipple(stipple.ToWxBitmap());
			}

			int GetWidth() const
			{
				return m_Pen.GetWidth();
			}
			void SetWidth(int width)
			{
				m_Pen.SetWidth(width);
			}

			size_t GetDashCount() const
			{
				return m_Pen.GetDashCount();
			}
			size_t GetDashes(Dash*& dashes) const
			{
				return m_Pen.GetDashes(&dashes);
			}
			std::vector<Dash> GetDashes() const
			{
				Dash* dashes = nullptr;
				int count = m_Pen.GetDashes(&dashes);
				if (dashes && count != 0)
				{
					return {dashes, dashes + static_cast<size_t>(count)};
				}
				return {};
			}
			void SetDashes(const Dash* dashes, size_t count)
			{
				m_Pen.SetDashes(static_cast<int>(count), dashes);
			}

			template<size_t N>
			void SetDashes(const Dash(&dashes)[N])
			{
				m_Pen.SetDashes(static_cast<int>(N), dashes);
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

			Pen& operator=(const Pen& other)
			{
				m_Pen = other.m_Pen;

				return *this;
			}
	};
}

namespace kxf::Drawing
{
	Pen GetStockPen(StockPen pen);
}
