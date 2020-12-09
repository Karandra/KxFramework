#include "stdafx.h"
#include "SVGImageHandler.h"
#include "../SVGImage.h"
#include "../BitmapImage.h"
#include "kxf/wxWidgets/StreamWrapper.h"

namespace kxf::Drawing::Private
{
	bool SVGImageHandler::DoCanRead(wxInputStream& stream)
	{
		wxWidgets::InputStreamWrapper wrapper(stream);

		SVGImage image;
		return image.Load(wrapper, ImageFormat::SVG);
	}
	int SVGImageHandler::DoGetImageCount(wxInputStream& stream)
	{
		return SVGImageHandler::DoCanRead(stream) ? 1 : 0;
	}

	SVGImageHandler::SVGImageHandler()
	{
		m_name = wxS("Scalable Vector Graphics");
		m_mime = wxS("image/svg+xml");
		m_extension = wxS("svg");
		m_type = Drawing::Private::NewWxBitmapType();
	}

	bool SVGImageHandler::LoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index)
	{
		if (image && index <= 0)
		{
			wxWidgets::InputStreamWrapper wrapper(stream);

			SVGImage svgImage;
			if (svgImage.Load(wrapper, ImageFormat::SVG))
			{
				const int dpi = image->GetOptionInt(wxIMAGE_OPTION_RESOLUTION);
				const int width = image->GetOptionInt(wxIMAGE_OPTION_MAX_WIDTH);
				const int height = image->GetOptionInt(wxIMAGE_OPTION_MAX_HEIGHT);

				svgImage.SetOption(ImageOption::DPI, dpi);
				*image = svgImage.Rasterize({width, height}).ToWxImage();
				return image->IsOk();
			}
		}
		return false;
	}
	bool SVGImageHandler::SaveFile(wxImage* image, wxOutputStream& stream, bool verbose)
	{
		// We can't save our raster image back to SVG
		return false;
	}
}
