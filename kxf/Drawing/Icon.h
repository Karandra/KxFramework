#pragma once
#include "Common.h"
#include "IGDIImage.h"
#include <wx/icon.h>

namespace kxf
{
	class KX_API Icon: public RTTI::ExtendInterface<Icon, IGDIImage>
	{
		KxRTTI_DeclareIID(Icon, {0x5da6784, 0xf20c, 0x4ad6, {0xb1, 0x3b, 0x3a, 0xa1, 0x2d, 0xd9, 0x66, 0x4a}});

		private:
			wxIcon m_Icon;

		public:
			Icon() = default;
			Icon(const wxIcon& other)
				:m_Icon(other)
			{
			}
			
			Icon(const Image& other);
			Icon(const Bitmap& other);
			Icon(const Cursor& other);
			Icon(const Icon& other)
				:m_Icon(other.m_Icon)
			{
			}

			Icon(const char* xbm, const Size& size)
				:m_Icon(xbm, size.GetWidth(), size.GetHeight())
			{
			}
			
			virtual ~Icon() = default;

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
			std::unique_ptr<IGDIObject> Clone() const override
			{
				return std::make_unique<Icon>(m_Icon);
			}

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// IGDIImage
			Size GetSize() const override
			{
				return m_Icon.IsOk() ? Size(m_Icon.GetSize()) : Size::UnspecifiedSize();
			}
			ColorDepth GetDepth() const override
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

			Cursor ToCursor(const Point& hotSpot = Point::UnspecifiedPosition()) const;
			Bitmap ToBitmap() const;
			Image ToImage() const;

			Icon ConvertToDisabled(Angle brightness = Angle::FromNormalized(1)) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			Icon& operator=(const Icon& other)
			{
				m_Icon = other.m_Icon;

				return *this;
			}
	};
}
