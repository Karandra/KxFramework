#include "KxStdAfx.h"
#include "GraphicsBitmap.h"
#include "Kx/Sciter/ScriptValue.h"
#include "Kx/Sciter/SciterAPI.h"
#include "Kx/Sciter/Internal.h"

namespace KxSciter
{
	std::optional<SCITER_IMAGE_ENCODING> MapImageEncoding(GraphicsBitmap::Format encoding)
	{
		switch (encoding)
		{
			case GraphicsBitmap::Format::PNG:
			{
				return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_PNG;
			}
			case GraphicsBitmap::Format::JPG:
			{
				return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_JPG;
			}
			case GraphicsBitmap::Format::WEBP:
			{
				return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_WEBP;
			}
			case GraphicsBitmap::Format::Raw:
			{
				return SCITER_IMAGE_ENCODING::SCITER_IMAGE_ENCODING_RAW;
			}
		};
		return std::nullopt;
	}
	SC_COLOR CreateSciterColor(const KxColor& color)
	{
		return GetGrapchicsAPI()->RGBA(color.GetR(), color.GetG(), color.GetB(), color.GetA());
	}
	bool DoGetImageInfo(HIMG image, wxSize& size, bool& usesAlpha)
	{
		UINT width = 0;
		UINT height = 0;
		BOOL nativeUsesAlpha = FALSE;
		if (GetGrapchicsAPI()->imageGetInfo(image, &width, &height, &nativeUsesAlpha) == GRAPHIN_OK)
		{
			size.SetWidth(width);
			size.SetHeight(height);
			usesAlpha = nativeUsesAlpha;
			return true;
		}
		return false;
	}
}

namespace KxSciter
{
	void GraphicsBitmap::Acquire(GraphicsBitmapHandle* handle)
	{
		Release();
		if (GetGrapchicsAPI()->imageAddRef(ToSciterImage(handle)) == GRAPHIN_OK)
		{
			m_Handle = handle;
		}
		else
		{
			m_Handle = nullptr;
		}
	}
	void GraphicsBitmap::Release()
	{
		if (m_Handle)
		{
			GetGrapchicsAPI()->imageRelease(ToSciterImage(m_Handle));
			m_Handle = nullptr;
		}
	}

	GraphicsBitmap::GraphicsBitmap(const wxSize& size, bool withAlpha)
	{
		HIMG image = nullptr;
		if (GetGrapchicsAPI()->imageCreate(&image, size.GetWidth(), size.GetHeight(), withAlpha) == GRAPHIN_OK)
		{
			Acquire(FromSciterImage(image));
		}
	}
	GraphicsBitmap::GraphicsBitmap(const wxImage& image)
	{
		wxMemoryOutputStream outputStream;
		if (image.SaveFile(outputStream, wxBITMAP_TYPE_PNG))
		{
			wxMemoryInputStream inputStream(outputStream);
			Load(inputStream);
		}
	}
	GraphicsBitmap::GraphicsBitmap(const wxBitmap& bitmap)
		:GraphicsBitmap(bitmap.ConvertToImage())
	{
	}
	GraphicsBitmap::GraphicsBitmap(const ScriptValue& value)
	{
		HIMG image = nullptr;
		if (GetGrapchicsAPI()->vUnWrapImage(ToSciterScriptValue(value.GetNativeValue()), &image))
		{
			Acquire(FromSciterImage(image));
		}
	}

	bool GraphicsBitmap::CreateFromPixmap(const wxSize& size, const char* pixmapData, bool withAlpha)
	{
		HIMG image = nullptr;
		if (GetGrapchicsAPI()->imageCreateFromPixmap(&image, size.GetWidth(), size.GetHeight(), withAlpha, reinterpret_cast<const BYTE*>(pixmapData)) == GRAPHIN_OK)
		{
			Acquire(FromSciterImage(image));
			return true;
		}
		return false;
	}
	bool GraphicsBitmap::Load(wxInputStream& stream)
	{
		std::vector<BYTE> buffer;
		buffer.resize(stream.GetLength());
		if (stream.ReadAll(buffer.data(), buffer.size()))
		{
			HIMG image = nullptr;
			if (GetGrapchicsAPI()->imageLoad(buffer.data(), buffer.size(), &image) == GRAPHIN_OK)
			{
				Acquire(FromSciterImage(image));
				return true;
			}
		}
		return false;
	}
	bool GraphicsBitmap::Save(wxOutputStream& stream, Format format, int quality) const
	{
		auto encoding = MapImageEncoding(format);
		if (IsOk() && encoding)
		{
			return GetGrapchicsAPI()->imageSave(ToSciterImage(m_Handle), [](void* context, const BYTE* data, UINT size) -> BOOL
			{
				wxOutputStream& stream = *reinterpret_cast<wxOutputStream*>(context);
				return stream.WriteAll(data, size);
			}, &stream, *encoding, quality) == GRAPHIN_OK;
		}
		return false;
	}

	bool GraphicsBitmap::Clear(const KxColor& color)
	{
		return GetGrapchicsAPI()->imageClear(ToSciterImage(m_Handle), CreateSciterColor(color)) == GRAPHIN_OK;
	}
	wxSize GraphicsBitmap::GetSize() const
	{
		wxSize size = wxDefaultSize;
		bool usesAlpha = false;
		DoGetImageInfo(ToSciterImage(m_Handle), size, usesAlpha);

		return size;
	}
	bool GraphicsBitmap::UsesAlpha() const
	{
		wxSize size = wxDefaultSize;
		bool usesAlpha = false;
		DoGetImageInfo(ToSciterImage(m_Handle), size, usesAlpha);

		return usesAlpha;
	}

	wxImage GraphicsBitmap::ConvertToImage() const
	{
		wxMemoryOutputStream outputStream;
		if (Save(outputStream, Format::PNG))
		{
			wxMemoryInputStream inputStream(outputStream);
			wxImage image(inputStream, wxBITMAP_TYPE_PNG);
			return image;
		}
		return {};
	}
	wxBitmap GraphicsBitmap::ConvertToBitmap() const
	{
		wxImage image = ConvertToImage();
		return wxBitmap(image, image.HasAlpha() ? 32 : -1);
	}
	ScriptValue GraphicsBitmap::ToScriptValue() const
	{
		ScriptValue value;
		if (GetGrapchicsAPI()->vWrapImage(ToSciterImage(m_Handle), ToSciterScriptValue(value.GetNativeValue())) == GRAPHIN_OK)
		{
			return value;
		}
		return {};
	}
}