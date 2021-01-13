#pragma once
#include "../Renderer.h"

namespace kxf::UI::DataView
{
	class KX_API ImageValueBase
	{
		protected:
			Size m_DefaultSize = Size::UnspecifiedSize();
			int m_ImageSpacing = 0;
			bool m_DrawInvalid = false;

		public:
			ImageValueBase() = default;

		public:
			int GetImageSpacing() const
			{
				return m_ImageSpacing;
			}
			void SetImageSpacing(int value)
			{
				m_ImageSpacing = value;
			}

			bool ShouldDrawInvalidImages() const
			{
				return m_DrawInvalid;
			}
			void DrawInvalidImages(bool value = true)
			{
				m_DrawInvalid = value;
			}

			Size GetDefaultImagesSize() const
			{
				return m_DefaultSize;
			}
			void SetDefaultImagesSize(const Size& value)
			{
				m_DefaultSize = value;
			}
	};
}

namespace kxf::UI::DataView
{
	class KX_API ImageValue: public ImageValueBase
	{
		protected:
			std::unique_ptr<IImage2D> m_Image;

		public:
			ImageValue() = default;
			ImageValue(const ImageValue&) = delete;
			ImageValue(ImageValue&&) = default;

			ImageValue(std::unique_ptr<IImage2D> image)
			{
				SetImage(std::move(image));
			}

		public:
			bool FromAny(Any value);

			bool HasImage() const
			{
				return m_Image != nullptr;
			}
			IImage2D* GetImage()
			{
				return m_Image.get();
			}
			const IImage2D* GetImage() const
			{
				return m_Image.get();
			}
			void SetImage(std::unique_ptr<IImage2D> image)
			{
				m_Image = std::move(image);
				if (m_Image && m_Image->IsNull())
				{
					m_Image = nullptr;
				}
			}
			void ClearImage()
			{
				m_Image = nullptr;
			}

		public:
			ImageValue& operator=(const ImageValue&) = delete;
			ImageValue& operator=(ImageValue&&) = default;
	};
}

namespace kxf::UI::DataView
{
	class KX_API ImageRenderer: public Renderer
	{
		private:
			ImageValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			ImageRenderer(FlagSet<Alignment> alignment = Alignment::CenterHorizontal|Alignment::CenterVertical)
				:Renderer(alignment)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return {};
			}
	};
}
