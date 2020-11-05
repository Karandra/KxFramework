#pragma once
#include "../Common.h"
#include "../Bitmap.h"
#include "../Image.h"

namespace kxf::Drawing::Private
{
	class AlphaBitmapRef final
	{
		private:
			Bitmap& m_Bitmap;

		public:
			AlphaBitmapRef(Bitmap& bitmap)
				:m_Bitmap(bitmap)
			{
			}

		public:
			const Bitmap& Get() const
			{
				return m_Bitmap;
			}
			Bitmap& Get()
			{
				return m_Bitmap;
			}
	};

	class AlphaImageRef final
	{
		private:
			Image& m_Image;

		public:
			AlphaImageRef(Image& image)
				:m_Image(image)
			{
				m_Image.EnableMask(false);
				m_Image.InitAlpha();
				m_Image.ClearData();
				m_Image.ClearAlpha();
			}

		public:
			const Image& Get() const
			{
				return m_Image;
			}
			Image& Get()
			{
				return m_Image;
			}
	};
}
