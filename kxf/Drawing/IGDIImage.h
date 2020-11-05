#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "ColorDepth.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class KX_API IGDIImage: public RTTI::ExtendInterface<IGDIImage, IGDIObject>
	{
		KxRTTI_DeclareIID(IGDIImage, {0x4bdebbfe, 0x9578, 0x4f2e, {0xa5, 0xb2, 0x4f, 0xf0, 0xff, 0x7, 0x87, 0x74}});

		public:
			virtual ~IGDIImage() = default;

		public:
			virtual Size GetSize() const = 0;
			int GetWidth() const
			{
				return GetSize().GetWidth();
			}
			int GetHeight() const
			{
				return GetSize().GetHeight();
			}
			virtual ColorDepth GetDepth() const = 0;

			virtual bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any) = 0;
			virtual bool Save(IOutputStream& stream, ImageFormat format) const = 0;
	};
}
