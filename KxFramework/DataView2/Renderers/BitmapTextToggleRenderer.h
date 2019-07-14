#pragma once
#include "KxFramework/KxFramework.h"
#include "TextRenderer.h"
#include "ToggleRenderer.h"
#include "BitmapTextRenderer.h"

namespace KxDataView2
{
	class KX_API BitmapTextToggleValue:
		public BitmapTextValue,
		public ToggleValue
	{
		public:
			BitmapTextToggleValue(const wxString& text = wxEmptyString,
								  const wxBitmap& bitmap = wxNullBitmap,
								  ToggleState state = ToggleState::None,
								  ToggleType type = ToggleType::None
			)
				:BitmapTextValue(text, bitmap), ToggleValue(state, type)
			{
			}

			BitmapTextToggleValue(const wxBitmap& bitmap)
				:BitmapTextToggleValue(wxEmptyString, bitmap)
			{
			}
			BitmapTextToggleValue(ToggleState state, ToggleType type = ToggleType::None)
				:BitmapTextToggleValue(wxEmptyString, wxNullBitmap, state, type)
			{
			}
			BitmapTextToggleValue(bool checked, ToggleType type = ToggleType::None)
				:BitmapTextToggleValue(wxEmptyString, wxNullBitmap, ToggleState::None, type)
			{
				SetChecked(checked);
			}
			BitmapTextToggleValue(bool checked, const wxString& text, const wxBitmap& bitmap = wxNullBitmap, ToggleType type = ToggleType::None)
				:BitmapTextToggleValue(text, bitmap, ToggleState::None, type)
			{
				SetChecked(checked);
			}
	
		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				BitmapTextValue::Clear();
				ToggleValue::Clear();
			}
	};
}

namespace KxDataView2
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
			wxAny OnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent = nullptr) override;

			bool SetValue(const wxAny& value);
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			BitmapTextToggleRenderer(int alignment = wxALIGN_INVALID)
				:Renderer(alignment), ToggleRendererBase(m_Value)
			{
			}
	};
}
