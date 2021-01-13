#pragma once
#include "../Renderer.h"
#include "kxf/General/WithOptions.h"
#include "TextRenderer.h"
#include "ImageRenderer.h"

namespace kxf::UI::DataView
{
	enum class ImageTextValueOption: uint32_t
	{
		None = 0,
		VCenterText = 1 << 0,
	};

	class KX_API ImageTextValue: public TextValue, public ImageValue, public WithOptions<ImageTextValueOption, ImageTextValueOption::None>
	{
		public:
			using Options = ImageTextValueOption;

		public:
			ImageTextValue() = default;
			ImageTextValue(std::unique_ptr<IImage2D> image)
				:ImageValue(std::move(image))
			{
			}
			ImageTextValue(String text)
				:TextValue(std::move(text))
			{
			}
			ImageTextValue(String text, std::unique_ptr<IImage2D> image)
				:TextValue(std::move(text)), ImageValue(std::move(image))
			{
			}

		public:
			bool FromAny(Any value);
	};
}
namespace kxf
{
	KxFlagSet_Declare(UI::DataView::ImageTextValueOption);
}

namespace kxf::UI::DataView
{
	class KX_API ImageTextRenderer: public Renderer
	{
		private:
			ImageTextValue m_Value;

		protected:
			bool SetDisplayValue(Any value) override;
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}

			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			ImageTextRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment)
			{
			}

		public:
			String GetDisplayText(Any value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(std::move(value)).GetText();
			}
	};
}
