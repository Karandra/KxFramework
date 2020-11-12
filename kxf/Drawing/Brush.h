#pragma once
#include "Common.h"
#include "Color.h"
#include "Bitmap.h"
#include "IGDIObject.h"
#include "Private/Common.h"
#include <wx/brush.h>

namespace kxf
{
	enum class StockBrush
	{
		Transparent,

		Black,
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
	class KX_API Brush: public RTTI::ExtendInterface<Brush, IGDIObject>
	{
		KxRTTI_DeclareIID(Brush, {0x8bc6b8e3, 0x5ae1, 0x46a9, {0x9f, 0x5a, 0xa7, 0x18, 0xcd, 0x3e, 0x5, 0x17}});

		private:
			wxBrush m_Brush;

		public:
			Brush() = default;
			Brush(const wxBrush& other)
				:m_Brush(other)
			{
			}
			Brush(const wxColour& color)
				:m_Brush(color, wxBRUSHSTYLE_SOLID)
			{
			}
			Brush(const Brush& other)
				:m_Brush(other.m_Brush)
			{
			}
			Brush(const Color& color)
				:m_Brush(color.ToWxColor(), wxBRUSHSTYLE_SOLID)
			{
			}
			Brush(const Bitmap& stippleBitmap)
				:m_Brush(stippleBitmap.ToWxBitmap())
			{
			}
			virtual ~Brush() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Brush.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				if (this == &other)
				{
					return true;
				}
				else if (auto object = other.QueryInterface<Brush>())
				{
					return m_Brush == object->m_Brush;
				}
				return false;
			}
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return std::make_unique<Brush>(m_Brush);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// Brush
			const wxBrush& ToWxBrush() const noexcept
			{
				return m_Brush;
			}
			wxBrush& ToWxBrush() noexcept
			{
				return m_Brush;
			}

			Color GetColor() const
			{
				return m_Brush.GetColour();
			}
			void SetColor(const Color& color)
			{
				m_Brush.SetColour(color.ToWxColor());
			}

			bool IsTransparent() const
			{
				return m_Brush.IsTransparent();
			}
			void SetTransparent()
			{
				m_Brush.SetStyle(wxBRUSHSTYLE_TRANSPARENT);
			}

			bool IsSolid() const
			{
				return m_Brush.GetStyle() == wxBRUSHSTYLE_SOLID;
			}
			void SetSolid()
			{
				m_Brush.SetStyle(wxBRUSHSTYLE_SOLID);
			}

			Bitmap GetStipple() const
			{
				const wxBitmap* stipple = m_Brush.GetStipple();
				if (stipple && stipple->IsOk())
				{
					return *stipple;
				}
				return {};
			}
			void SetStipple(const Bitmap& stipple)
			{
				m_Brush.SetStipple(stipple.ToWxBitmap());
				m_Brush.SetStyle(wxBRUSHSTYLE_STIPPLE);
			}

			bool IsHatch() const
			{
				return m_Brush.IsHatch();
			}
			HatchStyle GetHatchStyle() const
			{
				return Drawing::Private::MapHatchStyle(static_cast<wxHatchStyle>(m_Brush.GetStyle()));
			}
			void SetHatchStyle(HatchStyle style)
			{
				m_Brush.SetStyle(static_cast<wxBrushStyle>(Drawing::Private::MapHatchStyle(style)));
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

			Brush& operator=(const Brush& other)
			{
				m_Brush = other.m_Brush;

				return *this;
			}
	};
}

namespace kxf::Drawing
{
	Brush GetStockBrush(StockBrush brush);
}
