#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Editor.h"
#include "KxFramework/KxWithOptions.h"
class wxGenericColourDialog;

namespace KxDataView2
{
	enum class ColorEditorOptions
	{
		None = 0,

		GenericEditor = 1 << 0,
		FullEditor = 1 << 1,
		ShowAlpha = 1 << 2,
	};
}

namespace KxDataView2
{
	class KX_API ColorValue: public KxWithOptions<ColorEditorOptions, ColorEditorOptions::None>
	{
		public:
			using Options = ColorEditorOptions;

		protected:
			KxColor m_Color;
			std::array<KxColor, wxColourData::NUM_CUSTOM> m_PaletteColors;

		public:
			ColorValue(const KxColor& color = wxNullColour);

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				*this = {};
			}

			bool HasColor() const
			{
				return m_Color.IsValid();
			}
			KxColor GetColor() const
			{
				return m_Color;
			}
			void SetColor(const KxColor& color)
			{
				m_Color = color;
			}
			void ClearColor()
			{
				m_Color = wxNullColour;
			}
			
			size_t GetPaletteColorsLimit() const
			{
				return m_PaletteColors.size();
			}
			KxColor GetPaletteColor(size_t index) const
			{
				if (index < m_PaletteColors.size())
				{
					return m_PaletteColors[index];
				}
				return wxNullColour;
			}
			bool SetPaletteColor(size_t index, const KxColor& color)
			{
				if (index < m_PaletteColors.size())
				{
					m_PaletteColors[index] = color;
					return true;
				}
				return false;
			}
			void ClearPaletteColors()
			{
				m_PaletteColors.fill(wxNullColour);
			}

			wxColourData ToColorData() const;
			void FromColorData(const wxColourData& colorData);
	};
}

namespace KxDataView2
{
	class KX_API ColorEditor: public Editor
	{
		public:
			using Options = ColorEditorOptions;

		private:
			ColorValue m_Value;
			wxDialog* m_Dialog = nullptr;

		protected:
			wxWindow* CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
			wxAny GetValue(wxWindow* control) const override;
	};
}
