#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"
#include <KxFramework/KxWithImageList.h>

namespace Kx::DataView2
{
	class KX_API BitmapListRenderer: public Renderer
	{
		private:
			int m_Spacing = 0;
			bool m_SkipInvalidBitmaps = false;

		protected:
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		protected:
			virtual size_t GetBitmapCount() const = 0;
			virtual wxBitmap GetBitmap(size_t index) const = 0;

		public:
			BitmapListRenderer(int alignment = wxALIGN_INVALID)
				:Renderer(alignment)
			{
			}

		public:
			int GetSpacing() const
			{
				return m_Spacing;
			}
			void SetSpacing(int value)
			{
				m_Spacing = value;
			}
	
			bool ShouldSkipInvalidBitmaps() const
			{
				return m_SkipInvalidBitmaps;
			}
			void SkipInvalidBitmaps(bool value)
			{
				m_SkipInvalidBitmaps = value;
			}
	};
}

namespace Kx::DataView2
{
	class KX_API ImageListRenderer: public BitmapListRenderer, public KxWithImageList
	{
		protected:
			bool SetValue(const wxAny& value) override;
			wxSize GetCellSize() const override;
			size_t GetBitmapCount() const override
			{
				return HasImageList() ? GetImageList()->GetImageCount() : 0;
			}
			wxBitmap GetBitmap(size_t index) const override
			{
				return HasImageList() ? GetBitmap(index) : wxNullBitmap;
			}

		public:
			ImageListRenderer(int alignment = wxALIGN_INVALID)
				:BitmapListRenderer(alignment)
			{
			}
	};
}