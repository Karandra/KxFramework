#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
#include "ImageRenderer.h"

namespace kxf::UI::DataView
{
	class KX_API ImageListValue: public TextValue, public ImageValueBase
	{
		private:
			std::vector<std::unique_ptr<IImage2D>> m_Images;

		public:
			ImageListValue() = default;
			ImageListValue(const ImageListValue&) = delete;
			ImageListValue(ImageListValue&&) = default;

			ImageListValue(String text)
				:TextValue(std::move(text))
			{
			}
			ImageListValue(std::unique_ptr<IImage2D> image)
			{
				AddImage(std::move(image));
			}
			ImageListValue(String text, std::unique_ptr<IImage2D> image)
				:TextValue(std::move(text))
			{
				AddImage(std::move(image));
			}

		public:
			bool FromAny(Any value);

			bool HasImages() const
			{
				return !m_Images.empty();
			}
			size_t GetImageCount() const
			{
				return m_Images.size();
			}
			const IImage2D* GetBitmap(size_t index) const
			{
				if (index < m_Images.size())
				{
					return m_Images[index].get();
				}
				return nullptr;
			}
			void AddImage(std::unique_ptr<IImage2D> image)
			{
				m_Images.emplace_back(std::move(image));
			}
			void ClearImages()
			{
				m_Images.clear();
			}

		public:
			ImageListValue& operator=(const ImageListValue&) = delete;
			ImageListValue& operator=(ImageListValue&&) = default;
	};
}

namespace kxf::UI::DataView
{
	class KX_API ImageListRendererBase: public Renderer
	{
		private:
			TextValue& m_TextValue;
			ImageValueBase& m_BitmapValueBase;

		protected:
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		protected:
			virtual size_t GetImageCount() const = 0;
			virtual BitmapImage GetImage(size_t index) const = 0;

		public:
			ImageListRendererBase(TextValue& textValue, ImageValueBase& bitmapValueBase, FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment), m_TextValue(textValue), m_BitmapValueBase(bitmapValueBase)
			{
			}
	};
}

namespace kxf::UI::DataView
{
	class KX_API ImageListRenderer: public ImageListRendererBase
	{
		private:
			ImageListValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;

			size_t GetImageCount() const override
			{
				return m_Value.GetImageCount();
			}
			BitmapImage GetImage(size_t index) const override
			{
				if (auto image = m_Value.GetBitmap(index))
				{
					return image->ToBitmapImage();
				}
				return {};
			}

		public:
			ImageListRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:ImageListRendererBase(m_Value, m_Value, alignment)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(std::move(value)).GetText();
			}
	};
}
