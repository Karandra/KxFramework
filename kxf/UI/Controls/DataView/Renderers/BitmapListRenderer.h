#pragma once
#include "../Renderer.h"
#include "TextRenderer.h"
#include "BitmapRenderer.h"

namespace kxf::UI::DataView
{
	class KX_API BitmapListValue: public TextValue, public BitmapValueBase
	{
		private:
			std::vector<GDIBitmap> m_Bitmaps;

		public:
			BitmapListValue() = default;
			BitmapListValue(const String& text)
				:TextValue(text)
			{
			}
			BitmapListValue(const GDIBitmap& bitmap)
			{
				AddBitmap(bitmap);
			}
			BitmapListValue(const String& text, const GDIBitmap& bitmap)
				:TextValue(text)
			{
				AddBitmap(bitmap);
			}

		public:
			bool FromAny(const Any& value);
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
			const GDIBitmap& GetBitmap(size_t index) const
			{
				return index < m_Bitmaps.size() ? m_Bitmaps[index] : wxNullBitmap;
			}
			void AddBitmap(const GDIBitmap& bitmap)
			{
				m_Bitmaps.push_back(bitmap);
			}
			void ClearBitmaps()
			{
				m_Bitmaps.clear();
			}
	};
}

namespace kxf::UI::DataView
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
			virtual GDIBitmap GetBitmap(size_t index) const = 0;

		public:
			BitmapListRendererBase(TextValue& textValue, BitmapValueBase& bitmapValueBase, FlagSet<Alignment> alignment = Alignment::Invalid)
				:Renderer(alignment), m_TextValue(textValue), m_BitmapValueBase(bitmapValueBase)
			{
			}
	};
}

namespace kxf::UI::DataView
{
	class KX_API BitmapListRenderer: public BitmapListRendererBase
	{
		private:
			BitmapListValue m_Value;

		protected:
			bool SetValue(const Any& value) override;

			size_t GetBitmapCount() const override
			{
				return m_Value.GetBitmapsCount();
			}
			GDIBitmap GetBitmap(size_t index) const override
			{
				return m_Value.GetBitmap(index);
			}

		public:
			BitmapListRenderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:BitmapListRendererBase(m_Value, m_Value, alignment)
			{
			}
	
		public:
			String GetTextValue(const Any& value) const override
			{
				return FromAnyUsing<decltype(m_Value)>(value).GetText();
			}
	};
}
