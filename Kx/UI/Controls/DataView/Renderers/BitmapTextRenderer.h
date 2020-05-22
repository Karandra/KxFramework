#pragma once
#include "../Renderer.h"
#include "Kx/General/WithOptions.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace KxFramework::UI::DataView
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
			BitmapTextValue(const wxString& text)
				:TextValue(text)
			{
			}
			BitmapTextValue(const wxString& text, const wxBitmap& bitmap)
				:TextValue(text), BitmapValue(bitmap)
			{
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				TextValue::Clear();
				BitmapValue::Clear();
				WithOptions::RawSetOptions(BitmapTextValueOption::None);
			}
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::DataView::BitmapTextValueOption);
}

namespace KxFramework::UI::DataView
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
			BitmapTextRenderer(int alignment = wxALIGN_INVALID)
				:Renderer(alignment)
			{
			}
			
		public:
			wxString GetTextValue(const wxAny& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
