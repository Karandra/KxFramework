#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace KxFramework::UI::DataView
{
	class KX_API BitmapListValue: public TextValue, public BitmapValueBase
	{
		private:
			std::vector<wxBitmap> m_Bitmaps;

		public:
			BitmapListValue() = default;
			BitmapListValue(const String& text)
				:TextValue(text)
			{
			}
			BitmapListValue(const wxBitmap& bitmap)
			{
				AddBitmap(bitmap);
			}
			BitmapListValue(const String& text, const wxBitmap& bitmap)
				:TextValue(text)
			{
				AddBitmap(bitmap);
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				TextValue::Clear();
				BitmapValueBase::Clear();
				ClearBitmaps();
			}

			bool HasBitmaps() const
			{
				return !m_Bitmaps.empty();
			}
			size_t GetBitmapsCount() const
			{
				return m_Bitmaps.size();
			}
			const wxBitmap& GetBitmap(size_t index) const
			{
				return index < m_Bitmaps.size() ? m_Bitmaps[index] : wxNullBitmap;
			}
			void AddBitmap(const wxBitmap& bitmap)
			{
				m_Bitmaps.push_back(bitmap);
			}
			void ClearBitmaps()
			{
				m_Bitmaps.clear();
			}
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API BitmapListRendererBase: public Renderer
	{
		private:
			TextValue& m_TextValue;
			BitmapValueBase& m_BitmapValueBase;

		protected:
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		protected:
			virtual size_t GetBitmapCount() const = 0;
			virtual wxBitmap GetBitmap(size_t index) const = 0;

		public:
			BitmapListRendererBase(TextValue& textValue, BitmapValueBase& bitmapValueBase, FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment), m_TextValue(textValue), m_BitmapValueBase(bitmapValueBase)
			{
			}
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API BitmapListRenderer: public BitmapListRendererBase
	{
		private:
			BitmapListValue m_Value;

		protected:
			bool SetValue(const wxAny& value) override;

			size_t GetBitmapCount() const override
			{
				return m_Value.GetBitmapsCount();
			}
			wxBitmap GetBitmap(size_t index) const override
			{
				return m_Value.GetBitmap(index);
			}

		public:
			BitmapListRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:BitmapListRendererBase(m_Value, m_Value, alignment)
			{
			}
	
		public:
			String GetTextValue(const wxAny& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
