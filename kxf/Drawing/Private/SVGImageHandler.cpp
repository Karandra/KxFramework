#include "stdafx.h"
#include "SVGImageHandler.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include "kxf/wxWidgets/ClientObject.h"
#include <lunasvg/svgdocument.h>

namespace
{
	constexpr size_t g_RGBChannels = 3;
	constexpr size_t g_ChannelCount = g_RGBChannels + 1;
	constexpr double g_DefaultDPI = 96.0;

	bool DoLoad(lunasvg::SVGDocument& document, wxInputStream& stream)
	{
		using namespace kxf;

		wxWidgets::InputStreamWrapper wrapper(stream);
		IO::InputStreamReader reader(wrapper);

		return document.loadFromData(reader.ReadStdString(wrapper.GetSize().ToBytes()));
	}
}

namespace kxf::Drawing::Private
{
	bool SVGImageHandler::DoCanRead(wxInputStream& stream)
	{
		lunasvg::SVGDocument document;
		return DoLoad(document, stream);
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
		m_altExtensions.emplace_back(wxS("svgz"));
		m_type = static_cast<wxBitmapType>(ImageFormat::SVG);
	}

	bool SVGImageHandler::LoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index)
	{
		lunasvg::SVGDocument document;
		if (image && index <= 0 && DoLoad(document, stream))
		{
			const int dpi = image->GetOptionInt(wxIMAGE_OPTION_RESOLUTION);
			const int width = image->GetOptionInt(wxIMAGE_OPTION_MAX_WIDTH);
			const int height = image->GetOptionInt(wxIMAGE_OPTION_MAX_HEIGHT);

			lunasvg::Bitmap bitmap = document.renderToBitmap(width, height, dpi > 0 ? static_cast<double>(dpi) : g_DefaultDPI);
			if (const auto sourceData = bitmap.data())
			{
				// If we have no size specified set it to the SVG's default size
				const Geometry::BasicSize<size_t> actualSize = {width <= 0 ? bitmap.width() : width, height <= 0 ? bitmap.height() : height};

				if (image->Create(actualSize, false))
				{
					if (!image->HasAlpha())
					{
						image->InitAlpha();
					}

					auto targetRGB = image->GetData();
					auto targetAlpha = image->GetAlpha();

					size_t alphaCounter = 0;
					const size_t totalSize = actualSize.GetWidth() * actualSize.GetHeight() * g_ChannelCount;
					for (size_t i = 0; i < totalSize; i += g_ChannelCount)
					{
						const auto sourcePixel = sourceData + i;

						// Copy first 3 components for RGB data
						std::memcpy(targetRGB + i - alphaCounter, sourcePixel, g_RGBChannels);

						// Copy single component for alpha data
						*(targetAlpha + alphaCounter) = *(sourcePixel + g_RGBChannels);
						alphaCounter++;
					}
				}
				return true;
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
