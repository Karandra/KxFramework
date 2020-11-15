#pragma once
#include "Common.h"
#include "IGDIImage.h"
#include <wx/icon.h>

namespace kxf
{
	class KX_API GDIIcon: public RTTI::ExtendInterface<GDIIcon, IGDIImage>
	{
		KxRTTI_DeclareIID(GDIIcon, {0x5da6784, 0xf20c, 0x4ad6, {0xb1, 0x3b, 0x3a, 0xa1, 0x2d, 0xd9, 0x66, 0x4a}});

		private:
			wxIcon m_Icon;

		public:
			GDIIcon() = default;
			GDIIcon(const wxIcon& other)
				:m_Icon(other)
			{
			}

			GDIIcon(const Image& other);
			GDIIcon(const GDIBitmap& other);
			GDIIcon(const GDICursor& other);
			GDIIcon(const GDIIcon& other)
				:m_Icon(other.m_Icon)
			{
			}

			GDIIcon(const char* xbm, const Size& size)
				:m_Icon(xbm, size.GetWidth(), size.GetHeight())
			{
			}

			virtual ~GDIIcon() = default;

		public:
			// IGDIObject
			bool IsNull() const override
			{
				return !m_Icon.IsOk();
			}
			bool IsSameAs(const IGDIObject& other) const override
			{
				return m_Icon.GetHandle() == other.GetHandle();
			}
			std::unique_ptr<IGDIObject> CloneGDIObject() const override
			{
				return std::make_unique<GDIIcon>(m_Icon);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// IGDIImage
			Size GetSize() const override
			{
				return m_Icon.IsOk() ? Size(m_Icon.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetColorDepth() const override
			{
				return m_Icon.GetDepth();
			}

			bool Load(IInputStream& stream, ImageFormat format, int index);
			bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) override
			{
				return Load(stream, format, -1);
			}
			bool Save(IOutputStream& stream, ImageFormat format) const override;

			// Icon
			const wxIcon& ToWxIcon() const noexcept
			{
				return m_Icon;
			}
			wxIcon& ToWxIcon() noexcept
			{
				return m_Icon;
			}

			GDICursor ToCursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			GDIBitmap ToBitmap() const;
			Image ToImage() const;

			GDIIcon ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			GDIIcon& operator=(const GDIIcon& other)
			{
				m_Icon = other.m_Icon;

				return *this;
			}
	};
}
