#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
#include "ToggleRenderer.h"
#include "BitmapTextRenderer.h"

namespace kxf::UI::DataView
{
	class KX_API BitmapTextToggleValue: public BitmapTextValue, public ToggleValue
	{
		public:
			BitmapTextToggleValue(const String& text = {},
								  const GDIBitmap& bitmap = wxNullBitmap,
								  ToggleState state = ToggleState::None,
								  ToggleType type = ToggleType::None
			)
				:BitmapTextValue(text, bitmap), ToggleValue(state, type)
			{
			}

			BitmapTextToggleValue(const GDIBitmap& bitmap)
				:BitmapTextToggleValue({}, bitmap)
			{
			}
			BitmapTextToggleValue(ToggleState state, ToggleType type = ToggleType::None)
				:BitmapTextToggleValue({}, wxNullBitmap, state, type)
			{
			}
			BitmapTextToggleValue(bool checked, ToggleType type = ToggleType::None)
				:BitmapTextToggleValue({}, wxNullBitmap, ToggleState::None, type)
			{
				SetChecked(checked);
			}
			BitmapTextToggleValue(bool checked, const String& text, const GDIBitmap& bitmap = wxNullBitmap, ToggleType type = ToggleType::None)
				:BitmapTextToggleValue(text, bitmap, ToggleState::None, type)
			{
				SetChecked(checked);
			}
	
		public:
			bool FromAny(const Any& value);
			void Clear()
			{
				BitmapTextValue::Clear();
				ToggleValue::Clear();
			}
	};
}

namespace kxf::UI::DataView
{
	class KX_API BitmapTextToggleRenderer: public Renderer, public ToggleRendererBase
	{
		private:
			BitmapTextToggleValue m_Value;

		protected:
			bool HasActivator() const override
			{
				return true;
			}
			Any OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent = nullptr) override;

			bool SetValue(const Any& value);
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			BitmapTextToggleRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment), ToggleRendererBase(m_Value)
			{
			}
			
		public:
			String GetTextValue(const Any& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
