#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"

namespace Kx::DataView2
{
	class KX_API BitmapRenderer: public Renderer
	{
		public:
			static bool GetValueAsBitmap(const wxAny& value, wxBitmap& bitmap);
			static wxBitmap GetValueAsBitmap(const wxAny& value)
			{
				wxBitmap bitmap;
				GetValueAsBitmap(value, bitmap);
				return bitmap;
			}

		private:
			wxBitmap m_Bitmap;

		protected:
			bool SetValue(const wxAny& value) override;
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			BitmapRenderer(int alignment = wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL)
				:Renderer(alignment)
			{
			}
	};
}
