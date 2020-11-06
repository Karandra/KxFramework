#pragma once
#include "Common.h"
#include "IGDIImage.h"
#include <wx/bitmap.h>

namespace kxf
{
	class KX_API Bitmap: public RTTI::ExtendInterface<Bitmap, IGDIImage>
	{
		KxRTTI_DeclareIID(Bitmap, {0xd4c3e7be, 0xf0fd, 0x4c38, {0xa1, 0x94, 0x16, 0xb3, 0x9, 0xa, 0x34, 0xb5}});

		private:
			wxBitmap m_Bitmap;

		private:
			void Initialize();

		public:
			Bitmap()
			{
				Initialize();
			}
			Bitmap(const wxBitmap& other)
				:m_Bitmap(other)
			{
				Initialize();
			}

			Bitmap(const Icon& other);
			Bitmap(const Image& other);
			Bitmap(const Image& other, const wxDC& dc);
			Bitmap(const Cursor& other);
			Bitmap(const Bitmap& other)
				:m_Bitmap(other.m_Bitmap)
			{
				Initialize();
			}

			Bitmap(const char* xbm, const Size& size, ColorDepth depth = ColorDepthDB::BPP1)
				:m_Bitmap(xbm, size.GetWidth(), size.GetHeight(), depth ? depth.GetValue() : -1)
			{
				Initialize();
			}
			Bitmap(const Size& size, ColorDepth depth = {})
				:m_Bitmap(size.GetWidth(), size.GetHeight(), depth ? depth.GetValue() : -1)
			{
				Initialize();
			}
			Bitmap(const Size& size, const wxDC& dc)
				:m_Bitmap(size.GetWidth(), size.GetHeight(), dc)
			{
				Initialize();
			}

			virtual ~Bitmap() = default;

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
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return std::make_unique<Bitmap>(m_Bitmap);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// IGDIImage
			Size GetSize() const override
			{
				return m_Bitmap.IsOk() ? Size(m_Bitmap.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetDepth() const override
			{
				return m_Bitmap.GetDepth();
			}

			bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) override;
			bool Save(IOutputStream& stream, ImageFormat format) const override;

			// Bitmap
			const wxBitmap& ToWxBitmap() const noexcept
			{
				return m_Bitmap;
			}
			wxBitmap& ToWxBitmap() noexcept
			{
				return m_Bitmap;
			}

			Cursor ToCursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			Image ToImage() const;
			Icon ToIcon() const;

			Bitmap GetSubBitmap(const Rect& rect) const
			{
				return m_Bitmap.GetSubBitmap(rect);
			}
			Bitmap ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const
			{
				return m_Bitmap.ConvertToDisabled(static_cast<uint8_t>(brightness.ToNormalized() * 255));
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

			Bitmap& operator=(const Bitmap& other)
			{
				m_Bitmap = other.m_Bitmap;
				Initialize();

				return *this;
			}
	};
}
