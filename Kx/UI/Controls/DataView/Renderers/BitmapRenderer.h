#pragma once
#include "../Renderer.h"

namespace KxFramework::UI::DataView
{
	class KX_API BitmapValueBase
	{
		protected:
			int m_Spacing = 0;
			int m_DefaultWidth = -1;
			bool m_DrawInvalid = false;

		public:
			BitmapValueBase() = default;

		public:
			void Clear()
			{
				*this = {};
			}

			int GetSpacing() const
			{
				return m_Spacing;
			}
			void SetSpacing(int value)
			{
				m_Spacing = value;
			}

			bool ShouldDrawInvalidBitmaps() const
			{
				return m_DrawInvalid;
			}
			void DrawInvalidBitmaps(bool value = true)
			{
				m_DrawInvalid = value;
			}

			bool IsDefaultBitmapWidthSpecified() const
			{
				return m_DefaultWidth > 0;
			}
			int GetDefaultBitmapWidth() const
			{
				return m_DefaultWidth;
			}
			void SetDefaultBitmapWidth(int value)
			{
				m_DefaultWidth = value;
			}
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API BitmapValue: public BitmapValueBase
	{
		protected:
			wxBitmap m_Bitmap;

		public:
			BitmapValue(const wxBitmap& bitmap = wxNullBitmap)
				:m_Bitmap(bitmap)
			{
			}
			
		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				BitmapValueBase::Clear();
				ClearBitmap();
			}

			bool HasBitmap() const
			{
				return m_Bitmap.IsOk();
			}
			wxBitmap& GetBitmap()
			{
				return m_Bitmap;
			}
			const wxBitmap& GetBitmap() const
			{
				return m_Bitmap;
			}
			void SetBitmap(const wxBitmap& bitmap)
			{
				m_Bitmap = bitmap;
			}
			void ClearBitmap()
			{
				m_Bitmap = wxNullBitmap;
			}
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API BitmapRenderer: public Renderer
	{
		private:
			BitmapValue m_Value;

		protected:
			bool SetValue(const wxAny& value) override;
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			BitmapRenderer(FlagSet<Alignment> alignment = Alignment::CenterHorizontal|Alignment::CenterVertical)
				:Renderer(alignment)
			{
			}
			
		public:
			String GetTextValue(const wxAny& value) const override
			{
				return {};
			}
	};
}
