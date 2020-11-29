#pragma once
#include "Common.h"
#include "IGDIImage.h"
#include <wx/bitmap.h>

namespace kxf
{
	class GDIContext;
}

namespace kxf
{
	class KX_API GDIBitmap: public RTTI::ExtendInterface<GDIBitmap, IGDIImage>
	{
		KxRTTI_DeclareIID(GDIBitmap, {0xd4c3e7be, 0xf0fd, 0x4c38, {0xa1, 0x94, 0x16, 0xb3, 0x9, 0xa, 0x34, 0xb5}});

		private:
			wxBitmap m_Bitmap;

		private:
			void Initialize();

		public:
			GDIBitmap()
			{
				Initialize();
			}
			GDIBitmap(const wxBitmap& other)
				:m_Bitmap(other)
			{
				Initialize();
			}

			GDIBitmap(const GDIIcon& other);
			GDIBitmap(const Image& other);
			GDIBitmap(const Image& other, const GDIContext& dc);
			GDIBitmap(const GDICursor& other);
			GDIBitmap(const GDIBitmap& other)
				:m_Bitmap(other.m_Bitmap)
			{
				Initialize();
			}

			GDIBitmap(const char* xbm, const Size& size, ColorDepth depth = ColorDepthDB::BPP1)
				:m_Bitmap(xbm, size.GetWidth(), size.GetHeight(), depth ? depth.GetValue() : -1)
			{
				Initialize();
			}
			GDIBitmap(const Size& size, ColorDepth depth = {})
				:m_Bitmap(size.GetWidth(), size.GetHeight(), depth ? depth.GetValue() : -1)
			{
				Initialize();
			}
			GDIBitmap(const Size& size, const GDIContext& dc);

			virtual ~GDIBitmap() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Bitmap.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return m_Bitmap.GetHandle() == other.GetHandle();
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIBitmap>(m_Bitmap);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// IGDIImage
			Size GetSize() const override
			{
				return m_Bitmap.IsOk() ? Size(m_Bitmap.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Bitmap.GetDepth();
			}

			bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) override;
			bool Save(IOutputStream& stream, ImageFormat format) const override;

			// GDIBitmap
			const wxBitmap& ToWxBitmap() const noexcept
			{
				return m_Bitmap;
			}
			wxBitmap& ToWxBitmap() noexcept
			{
				return m_Bitmap;
			}

			GDICursor ToCursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			Image ToImage() const;
			GDIIcon ToIcon() const;

			GDIBitmap GetSubBitmap(const Rect& rect) const
			{
				return m_Bitmap.GetSubBitmap(rect);
			}
			GDIBitmap ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const
			{
				return m_Bitmap.ConvertToDisabled(static_cast<uint8_t>(brightness.ToNormalized() * 255));
			}
			void UpdateAlpha()
			{
				m_Bitmap.MSWUpdateAlpha();
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

			GDIBitmap& operator=(const GDIBitmap& other)
			{
				m_Bitmap = other.m_Bitmap;
				Initialize();

				return *this;
			}
	};
}
