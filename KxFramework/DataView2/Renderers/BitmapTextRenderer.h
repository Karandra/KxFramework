#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace KxDataView2
{
	enum class BitmapTextValueOptions
	{
		None = 0,
		VCenterText = 1 << 0,
	};

	class KX_API BitmapTextValue:
		public TextValue,
		public BitmapValue,
		public KxWithOptions<BitmapTextValueOptions, BitmapTextValueOptions::None>
	{
		public:
			using Options = BitmapTextValueOptions;

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
				KxWithOptions::SetOptionsValue(BitmapTextValueOptions::None);
			}
	};
}

namespace KxDataView2
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
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

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
