#pragma once
#include "Common.h"
#include "../IImageHandler.h"
#include <wx/image.h>

namespace kxf::Drawing::Private
{
	class SVGImageHandler final: public wxImageHandler, public IImageHandler
	{
		protected:
			// wxImageHandler
			bool DoCanRead(wxInputStream &stream) override;
			int DoGetImageCount(wxInputStream &stream) override;

		public:
			SVGImageHandler();

		public:
			// wxImageHandler
			bool LoadFile(wxImage *image, wxInputStream &stream, bool verbose=true, int index=-1) override;
			bool SaveFile(wxImage* image, wxOutputStream& stream, bool verbose = true) override;

		public:
			// IImageHandler
			std::unique_ptr<IImage2D> CreateImage();

			size_t GetSubImageCount(IInputStream& stream) override;
			bool LoadImage(IImage2D& image, IInputStream& stream, size_t index) override;
			bool SaveImage(const IImage2D& image, IOutputStream& stream) const override;
	};
}
