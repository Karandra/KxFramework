#pragma once
#include "Common.h"
#include "Color.h"
#include "Bitmap.h"
#include "IGDIObject.h"
#include "Private/Common.h"
#include <wx/brush.h>

namespace kxf
{
	enum class StockPen
	{
		Transparent,

		Black,
		BlackDashed,
		White,
		Cyan,
		Blue,
		Red,
		Green,
		Yellow,
		Gray,
		LightGray,
		MediumGray
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
			Pen(const wxColour& color)
				:m_Pen(color, wxPENSTYLE_SOLID)
			{
			}
			Pen(const Pen& other)
				:m_Pen(other.m_Pen)
			{
			}
			Pen(const Color& color)
				:m_Pen(color.ToWxColor(), wxPENSTYLE_SOLID)
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

			bool IsSolid() const
			{
				return m_Pen.GetStyle() == wxPENSTYLE_SOLID;
			}
			void SetSolid()
			{
				m_Pen.SetStyle(wxPENSTYLE_SOLID);
			}

			bool IsTransparent() const
			{
				return m_Pen.IsTransparent();
			}
			void SetTransparent()
			{
				m_Pen.SetStyle(wxPENSTYLE_TRANSPARENT);
			}

			bool IsHatch() const
			{
				const auto style = ToInt(m_Pen.GetStyle());
				return style >= wxPENSTYLE_FIRST_HATCH && style <= wxPENSTYLE_LAST_HATCH;
			}
			HatchStyle GetHatchStyle() const
			{
				return Drawing::Private::MapHatchStyle(static_cast<wxHatchStyle>(m_Pen.GetStyle()));
			}
			void SetHatchStyle(HatchStyle style)
			{
				m_Pen.SetStyle(static_cast<wxPenStyle>(Drawing::Private::MapHatchStyle(style)));
			}

			LineJoin GetJoin() const
			{
				return Drawing::Private::MapLineJoin(m_Pen.GetJoin());
			}
			void SetJoin(LineJoin join)
			{
				m_Pen.SetJoin(Drawing::Private::MapLineJoin(join));
			}

			LineCap GetCap() const
			{
				return Drawing::Private::MapLineCap(m_Pen.GetCap());
			}
			void SetJoin(LineCap cap)
			{
				m_Pen.SetCap(Drawing::Private::MapLineCap(cap));
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

			DashStyle GetDashStyle() const
			{
				return Drawing::Private::MapDashStyle(static_cast<wxDeprecatedGUIConstants>(m_Pen.GetStyle()));
			}
			void SetDashStyle(DashStyle style)
			{
				if (auto wxStyle = Drawing::Private::MapDashStyle(style))
				{
					m_Pen.SetStyle(static_cast<wxPenStyle>(*wxStyle));
				}
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

			template<size_t N>
			void SetDashes(const std::array<Dash, N>& dashes)
			{
				m_Pen.SetDashes(static_cast<int>(dashes.size()), dashes.data());
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
