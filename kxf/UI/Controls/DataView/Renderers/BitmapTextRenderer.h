#pragma once
#include "../Renderer.h"
#include "kxf/General/WithOptions.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace kxf::UI::DataView
{
	enum class BitmapTextValueOption
	{
		None = 0,
		VCenterText = 1 << 0,
	};

	class KX_API BitmapTextValue:
		public TextValue,
		public BitmapValue,
		public WithOptions<BitmapTextValueOption, BitmapTextValueOption::None>
	{
		public:
			using Options = BitmapTextValueOption;

		public:
			BitmapTextValue() = default;
			BitmapTextValue(const wxBitmap& bitmap)
				:BitmapValue(bitmap)
			{
			}
			BitmapTextValue(const String& text)
				:TextValue(text)
			{
			}
			BitmapTextValue(const String& text, const wxBitmap& bitmap)
				:TextValue(text), BitmapValue(bitmap)
			{
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				TextValue::Clear();
				BitmapValue::Clear();
				WithOptions::SetOptionFlags(BitmapTextValueOption::None);
			}
	};
}
namespace kxf
{
	Kx_DeclareFlagSet(UI::DataView::BitmapTextValueOption);
}

namespace kxf::UI::DataView
{
	class KX_API BitmapTextRenderer: public Renderer
	{
		private:
			BitmapTextValue m_Value;

		protected:
			bool SetValue(const wxAny& value) override;
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			BitmapTextRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment)
			{
			}
			
		public:
			String GetTextValue(const wxAny& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
