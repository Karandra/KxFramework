#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"

namespace KxDataView2
{
	class KX_API BitmapTextValue
	{
		private:
			wxBitmap m_Bitmap;
			wxString m_Text;
			bool m_VCenterText = true;

		public:
			BitmapTextValue(const wxString& text = wxEmptyString, const wxBitmap& bitmap = wxNullBitmap)
				:m_Text(text), m_Bitmap(bitmap)
			{
			}

		public:
			bool HasText() const
			{
				return !m_Text.IsEmpty();
			}
			wxString& GetText()
			{
				return m_Text;
			}
			const wxString& GetText() const
			{
				return m_Text;
			}
			void SetText(const wxString& text)
			{
				m_Text = text;
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

			void VCenterText(bool value = true)
			{
				m_VCenterText = value;
			}
			bool ShouldVCenterText() const
			{
				return m_VCenterText;
			}
	};
}

namespace KxDataView2
{
	class KX_API BitmapTextRenderer: public Renderer
	{
		private:
			BitmapTextValue m_Value;

		protected:
			virtual bool SetValue(const wxAny& value) override;
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			BitmapTextRenderer(int alignment = wxALIGN_INVALID)
				:Renderer(alignment)
			{
			}
	};
}
