#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"
#include <KxFramework/KxWithImageList.h>

namespace KxDataView2
{
	class KX_API BitmapListRendererBase: public Renderer
	{
		private:
			wxString m_Text;
			int m_Spacing = 0;
			bool m_SkipInvalidBitmaps = false;

		protected:
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

			bool SetValue(const wxAny& value) override;

		protected:
			virtual size_t GetBitmapCount() const = 0;
			virtual const wxBitmap& GetBitmap(size_t index) const = 0;

		public:
			BitmapListRendererBase(int alignment = wxALIGN_INVALID)
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

namespace KxDataView2
{
	class KX_API BitmapListRenderer: public BitmapListRendererBase
	{
		private:
			std::vector<wxBitmap> m_Bitmaps;

		protected:
			bool SetValue(const wxAny& value) override;

			size_t GetBitmapCount() const override
			{
				return m_Bitmaps.size();
			}
			const wxBitmap& GetBitmap(size_t index) const override
			{
				return index < m_Bitmaps.size() ? m_Bitmaps[index] : wxNullBitmap;
			}

		public:
			BitmapListRenderer(int alignment = wxALIGN_INVALID)
				:BitmapListRendererBase(alignment)
			{
			}
	};
}

namespace KxDataView2
{
	class KX_API ImageListRenderer: public BitmapListRendererBase, public KxWithImageList
	{
		protected:
			bool SetValue(const wxAny& value) override;

			size_t GetBitmapCount() const override
			{
				return HasImageList() ? GetImageList()->GetImageCount() : 0;
			}
			const wxBitmap& GetBitmap(size_t index) const override
			{
				return HasImageList() ? GetBitmap(index) : wxNullBitmap;
			}

		public:
			ImageListRenderer(int alignment = wxALIGN_INVALID)
				:BitmapListRendererBase(alignment)
			{
			}
	};
}