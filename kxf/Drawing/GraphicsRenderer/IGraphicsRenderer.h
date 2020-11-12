#pragma once
#include "Common.h"

namespace kxf
{
	class wxWindow;

	class String;
	class Version;
	class Bitmap;
	class Image;

	class GDIContext;
	class IGraphicsContext;
}
namespace kxf::Drawing
{
	class GDIMemoryContextBase;
	class GDIWindowContextBase;
}

namespace kxf
{
	class KX_API IGraphicsRenderer: public RTTI::Interface<IGraphicsRenderer>
	{
		KxRTTI_DeclareIID(IGraphicsRenderer, {0xf8abb3f6, 0xee8d, 0x49d0, {0x96, 0xe5, 0x2f, 0xd6, 0xaa, 0x3c, 0x5d, 0x6f}});

		public:
			virtual ~IGraphicsRenderer() = default;

		public:
			virtual String GetName() const = 0;
			virtual Version GetVersion() const = 0;

			virtual std::unique_ptr<IGraphicsContext> CreateContext(Bitmap& bitmap) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateContext(Image& image) = 0;

			virtual std::unique_ptr<IGraphicsContext> CreateContext(wxWindow& window) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateContext(GDIContext& dc) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateContext(Drawing::GDIWindowContextBase& dc) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateContext(Drawing::GDIMemoryContextBase& dc) = 0;
			virtual std::unique_ptr<IGraphicsContext> CreateMeasuringContext() = 0;
	};
}
