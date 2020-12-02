#pragma once
#include "Common.h"
#include <wx/image.h>

namespace kxf::Drawing::Private
{
	class SVGImageHandler final: public wxImageHandler
	{
		protected:
			bool DoCanRead(wxInputStream &stream) override;
			int DoGetImageCount(wxInputStream &stream) override;

		public:
			SVGImageHandler();

		public:
			bool LoadFile(wxImage *image, wxInputStream &stream, bool verbose=true, int index=-1) override;
			bool SaveFile(wxImage* image, wxOutputStream& stream, bool verbose = true) override;
	};
}
