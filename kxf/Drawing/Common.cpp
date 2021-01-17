#include "stdafx.h"
#include "Common.h"
#include "IImageHandler.h"
#include "BitmapImage.h"
#include "kxf/wxWidgets/StreamWrapper.h"
#include "kxf/Utility/ScopeGuard.h"
#include <wx/image.h>

namespace kxf::Drawing
{
	std::unique_ptr<IImage2D> LoadImage(IInputStream& stream, size_t index)
	{
		if (stream.IsSeekable())
		{
			// Save the offset and restore it before every loop iteration
			const auto initialOffset = stream.TellI();

			for (wxObject* objectWx: wxImage::GetHandlers())
			{
				Utility::ScopeGuard atExit = [&]()
				{
					stream.SeekI(initialOffset, IOStreamSeek::FromStart);
				};

				IObject* objectKxf = dynamic_cast<IObject*>(objectWx);
				if (object_ptr<IImageHandler> handlerKxf; objectKxf && objectKxf->QueryInterface(handlerKxf))
				{
					auto image = handlerKxf->CreateImage();
					if (handlerKxf->LoadImage(*image, stream, index))
					{
						return image;
					}
				}
				else
				{
					wxImageHandler& handlerWx = static_cast<wxImageHandler&>(*objectWx);

					wxImage image;
					wxWidgets::InputStreamWrapperWx wrapper(stream);
					if (handlerWx.LoadFile(&image, wrapper, false, index != std::numeric_limits<size_t>::max() ? static_cast<int>(index) : -1))
					{
						return std::make_unique<BitmapImage>(std::move(image));
					}
				}
			}
		}
		return nullptr;
	}
}
