#pragma once
#include "../Common.h"
#include <wx/bitmap.h>
#include <wx/image.h>

namespace kxf::Drawing::Private
{
	class AlphaBitmapRef final
	{
		private:
			wxBitmap& m_Bitmap;

		public:
			AlphaBitmapRef(wxBitmap& bitmap)
				:m_Bitmap(bitmap)
			{
				m_Bitmap.UseAlpha(true);
			}

		public:
			const wxBitmap& Get() const
			{
				return m_Bitmap;
			}
			wxBitmap& Get()
			{
				return m_Bitmap;
			}
	};

	class AlphaImageRef final
	{
		private:
			wxImage& m_Image;

		private:
			void ClearColor()
			{
				const size_t length = (size_t)m_Image.GetWidth() * (size_t)m_Image.GetHeight() * size_t(3);
				memset(m_Image.GetData(), 0, length);
			}
			void ClearAlpha()
			{
				const size_t length = (size_t)m_Image.GetWidth() * (size_t)m_Image.GetHeight();
				memset(m_Image.GetAlpha(), 0, length);
			}

		public:
			AlphaImageRef(wxImage& image)
				:m_Image(image)
			{
				m_Image.SetMask(false);
				m_Image.SetPalette(wxNullPalette);
				if (!m_Image.HasAlpha())
				{
					m_Image.InitAlpha();
				}

				ClearColor();
				ClearAlpha();
			}

		public:
			const wxImage& Get() const
			{
				return m_Image;
			}
			wxImage& Get()
			{
				return m_Image;
			}
	};
}
