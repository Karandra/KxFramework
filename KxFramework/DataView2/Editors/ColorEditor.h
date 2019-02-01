#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Editor.h"
#include "KxFramework/KxWithOptions.h"
class wxGenericColourDialog;

namespace KxDataView2
{
	enum class ColorEditorStyle
	{
		None = 0,

		UseGeneric = 1 << 0,
		ShowAlpha = 1 << 1,
		Full = 1 << 2,
	};
}

namespace KxDataView2
{
	class KX_API ColorEditor: public Editor, public KxWithOptions<ColorEditorStyle, ColorEditorStyle::None>
	{
		public:
			static bool GetValueAsColor(const wxAny& value, KxColor& color);
			static KxColor GetValueAsColor(const wxAny& value)
			{
				KxColor color;
				GetValueAsColor(value, color);
				return color;
			}

		private:
			wxColourData m_ColorData;
			wxDialog* m_Dialog = nullptr;

		protected:
			wxWindow* CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
			wxAny GetValue(wxWindow* control) const override;

		public:
			wxColour GetCustomColor(size_t index) const;
			void SetCustomColor(size_t index, const wxColour& color);
	};
}
